#include "input_stage.h"

volatile bool input_stage::data_rdy_adc1=false;

input_data::input_data():
    adc1_mode(0),
    adc1_data_mv(0.0f),
    adc1_data_a(0.0f),
    adc1_data_ma(0.0f),
    adc1_num_samples(0),
    adc1_timestamp(0),
    adc2_data_mv(0.0f),
    adc2_data_v(0.0f),
    adc2_num_samples(0),
    adc2_timestamp(0),
    calculated_power(0.0f)
{}
input_config::input_config():
    current_limit(DEFAULT_CURRENT_LIMIT),
    power_limit(DEFAULT_POWER_LIMIT),
    delay(DEFAULT_DELAY),
    motor_mode(DEFAULT_MOTOR_MODE),
    sample_rate_adc1(DEFAULT_SAMPLE_RATE_ADC1),
    sample_rate_adc2(DEFAULT_SAMPLE_RATE_ADC2),
    op_gain(DEFAULT_OP_GAIN),
    shunt_value(DEFAULT_SHUNT_VALUE),
    voltage_divider(DEFAULT_VOLTAGE_DIVIDER),
    level_low(DEFAULT_LEVEL_LOW), level_high(DEFAULT_LEVEL_HIGH),
    adc1_address(ADC1_I2C_ADDRESS), adc2_address(ADC2_I2C_ADDRESS)
{}
input_state::input_state() :
    measure_mode(RANGE_LOW),
    running_measurement(false),

    timestamp_begin(0),
    timestamp_output(0),
    timestamp_overshoot(0),

    state_output(OUTPUT_ACTIVE)
{}

input_stage::input_stage(input_config &config_data) : 
    config(config_data),
    adc1(config.adc1_address), adc2(config.adc2_address)
{

    //Hier sind Pullups nötig, wenn die Verbindung am Arduino abfällt -> Leitung offen (undefiniert)
    pinMode(ALARM_PIN_ADC1, INPUT_PULLUP);
    
    pinMode(RELAY_PIN_1, OUTPUT);
    pinMode(RELAY_PIN_2, OUTPUT);
    RELAY_TURNOFF();

    //Prüfen ob "Wire" schon einmal initialisiert wurde
    if(!TWCR) {
        Wire.begin();
        Wire.setClock(I2C_SPEED);
    }

    #if DEBUG
        if(!Serial) {
            Serial.begin(9600);
            while (!Serial);
        }
        Serial.println("Debugging started...");
        Serial.println("__________________\n\n");
    #endif

    //Hier noch u.A. Fehlerüberprüfung einfügen, falls keine Verbindung hergestellt werden kann
    errors.adc1_error = !adc1.init();
    errors.adc2_error = !adc2.init();

    #if DEBUG
    if(errors.adc1_error)
        { Serial.println("ADC1 Initialization failed!"); }
    if(errors.adc2_error)
        { Serial.println("ADC2 Initialization failed!"); }
    #endif

    adc1.setVoltageRange_mV(ADC1_RANGE_DIRECT);
    adc1.setCompareChannels(ADC1_PINS_DIRECT);

    adc2.setVoltageRange_mV(ADC2_RANGE_DIRECT);
    adc2.setCompareChannels(ADC2_PINS_DIRECT);

    adc1.setConvRate(config.sample_rate_adc1);
    adc2.setConvRate(config.sample_rate_adc2);

    /* Liegen dem ADC die angeforderten Daten vor, wird eine fallende Flanke an "ALARM_PIN" ausgelöst, ein Interrupt getriggert
    * und somit die Datenverarbeitung gestartet
    * 
    * Anzahl der gelesenen Datenpakete ist direkt proportional zur eingestellten SAMPLERATE
    */

    adc1.setAlertPinMode(ADS1115_ASSERT_AFTER_1);
    adc1.setAlertPol(ADS1115_ACT_LOW);
    adc1.setAlertPinToConversionReady();

    adc2.setAlertPinMode(ADS1115_ASSERT_AFTER_1);
    adc2.setAlertPol(ADS1115_ACT_LOW);
    adc2.setAlertPinToConversionReady();
}

void input_stage::capture_data()
{
    //Sollte noch keine Messreihe gestartet sein hier den Modus initialisieren und starten
    if (!state.running_measurement)
    {
        start_capture();
        return;
    }

    //ADC1 und ADC2 auslesen
    if (data_rdy_adc1)
    { read_adc1(); read_adc2(); }

    //Wenn keine Fehler vorliegen, Leistung berechnen.
    if (!errors.range_error_adc1 && !errors.range_error_adc2)
    { data.calculated_power = (data.adc1_data_a * data.adc2_data_v); }

    //Prüft mögliche Strom/Leistungsüberschreitungen und deaktiviert bei Bedarf beide Relais
    check_limits();

    // Prüft mögliche Messbereichsüberschreitungen beim ADC1, passt bei Bedarf den Bereich an und gibt Debug-Meldungen aus
    // Steht ganz am Ende weil der Code hier relativ Performance-intensiv ist
    debug_display_data();
    check_range();

    //Neue Messung wird eingeleitet
    data_rdy_adc1 = false;
    adc2.startSingleMeasurement();
    adc1.startSingleMeasurement();

    return;
}

void input_stage::read_adc1() {

  float temp_data = adc1.getResult_mV();
  errors.range_error_adc1 = temp_data < 0.0f;
  data.adc1_num_samples++;

  data.adc1_data_mv   = temp_data / ((state.measure_mode == RANGE_HIGH) ? config.op_gain : 1.0f);
  data.adc1_data_ma   = (data.adc1_data_mv / (float)config.shunt_value) * 1000.0f;
  data.adc1_data_a    =  (data.adc1_data_mv / (float)config.shunt_value);
  data.adc1_mode      = state.measure_mode;
  data.adc1_timestamp = millis();
}

void input_stage::read_adc2() {
  float temp_data = (adc2.getResult_mV() * config.voltage_divider);
  temp_data = abs(temp_data);
  errors.range_error_adc2 = false;
  //errors.range_error_adc2 = (temp_data < 0.0f);
  data.adc2_num_samples++;

  data.adc2_data_mv = temp_data;
  data.adc2_data_v = (temp_data / 1000.0f);
  data.adc2_timestamp = millis();
}

void input_stage::check_limits()
{
    bool current_overshoot = errors.range_error_adc1 || (data.adc1_data_ma >= config.current_limit);
    bool power_overshoot   = (config.power_limit != NO_LIMIT) && (errors.range_error_adc2 || (data.calculated_power > config.power_limit));
    bool kill_output = current_overshoot || power_overshoot;

    //Keine Bereichsüberschreitung gefunden -> Messung wird normal fortgesetzt
    if(!kill_output)
    { state.timestamp_overshoot = 0.0f; return; }

    //-----------> Bereichsüberschreitung wird bearbeitet

        //Motormodus
        if(kill_output && config.motor_mode && ((millis() - state.timestamp_begin) <= DEFAULT_MOTOR_MODE_DELAY))
            { kill_output = false; }

        //Verzögerung beim Auslösen
        if(kill_output && config.delay != DELAY_OFF) {
            //
            if(state.timestamp_overshoot==0.0f)
                {state.timestamp_overshoot=millis();}

            if( (millis() - state.timestamp_overshoot) < config.delay)
                { kill_output = false; }
        }

    // -----------> Ausgang wird jetzt nach Auswertung u.U. abgeschalten
    if(kill_output) {
        RELAY_TURNOFF();
        
        if(current_overshoot) { state.state_output |= OUTPUT_DISABLED_CURRENT; }
        if(power_overshoot)   { state.state_output |= OUTPUT_DISABLED_POWER; }
    }
}

void input_stage::check_range() {
    if (state.measure_mode == RANGE_LOW && !errors.range_error_adc1)
  {
       //Messdaten außerhalb des niedrigen Bereichs, Messbereich für die nächste Messung umschalten
      if (!(data.adc1_data_mv >= config.level_low && data.adc1_data_mv <= config.level_high))
      {
          state.measure_mode = RANGE_HIGH;
          adc1.setVoltageRange_mV(ADC1_RANGE_OPAMP);
          adc1.setCompareChannels(ADC1_PINS_OPAMP);

          #if DEBUG
            Serial.println("ADJUSTING ADC1 RANGE: OP-AMP");
          #endif
      }
  }
  else if (state.measure_mode == RANGE_HIGH && !errors.range_error_adc1)
  {
       //Messdaten außerhalb des hohen Bereichs, Messbereich für die nächste Messung umschalten
      if (data.adc1_data_mv >= config.level_low && data.adc1_data_mv <= config.level_high)
      {
          state.measure_mode = RANGE_LOW;
          adc1.setVoltageRange_mV(ADC1_RANGE_DIRECT);
          adc1.setCompareChannels(ADC1_PINS_DIRECT);

          #if DEBUG
            Serial.println("ADJUSTING ADC1 RANGE: DIRECT");
          #endif
      }
  }
}
void input_stage::start_capture() {
    state.running_measurement = true;

    attachInterrupt(digitalPinToInterrupt(ALARM_PIN_ADC1), conversion_rdy_adc1, FALLING);

    #if WD_ENABLE
        //Watchdog löst aus, wenn 0.5s kein Signal mehr aus dem ALARM-Pin vom ADC1 kommt
        cli();
            asm("WDR");
            WDTCSR |= (1<<WDCE) | (1<<WDE);
            WDTCSR =  (1<<WDP1) | (1<<WDP2) | (1<<WDE);
        sei();
    #endif

    state.state_output = OUTPUT_ACTIVE;
    state.timestamp_begin = millis();
    RELAY_TURNON();

    adc1.startSingleMeasurement();
    adc2.startSingleMeasurement();
}
void input_stage::stop_capture() {
    adc1.setVoltageRange_mV(ADC1_RANGE_DIRECT);
    adc1.setCompareChannels(ADC1_PINS_DIRECT);

    detachInterrupt(digitalPinToInterrupt(ALARM_PIN_ADC1));

    #if WD_ENABLE
        //Watchdog löst aus, wenn 0.5s kein Signal mehr aus dem ALARM-Pin vom ADC1 kommt
        cli();
            asm("WDR");
            WDTCSR |= (1<<WDCE) | (1<<WDE);
            WDTCSR = 0x0;
        sei();
    #endif

    state = input_state();
    RELAY_TURNOFF();
}
void input_stage::debug_display_data()
{
#if (DEBUG && ADC_VERBOSE)
    Serial.println(" ###### ADC_1 ######");
    Serial.print(data.adc1_data_ma, 2);
    Serial.print("mA\n");
    Serial.print(data.adc1_data_mv, 2);
    Serial.print("mV\n\n");
    Serial.println("-------------\n");


    Serial.println(" ###################");
    Serial.println(" ###### ADC_2 ######");
    Serial.print(data.adc2_data_mv, 2);
    Serial.print("mV\n");
    Serial.print(data.calculated_power, 2);
    Serial.print("W\n\n");
#endif
}
void input_stage::debug_overcurrent_msg() {

        #if DEBUG
                Serial.print("OVERSHOOT\n");
                Serial.print(config.current_limit);
                Serial.print(" mA\nexceeded!");
        #endif

}