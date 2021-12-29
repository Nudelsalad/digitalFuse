#ifndef INPUT_STAGE
#define INPUT_STAGE

#include "defines.h"

#include <Wire.h>
#include <Arduino.h>
#include <ADS1115_WE.h>

/*
        Beispiel zum Arbeiten mit der Klasse "input_config"

        > Anlegen der globalen Variable; kann u.U. noch geändert werden. Ganz zufrieden bin ich mit der Lösung noch nicht
            input_stage* stage1;

        > Einmalige Initialisierung beim Starten des Geräts - noch keine Erfassung der Daten
        > Durch den Default-Konstruktor von "input_config" muss für die Standard-Einstellungen nichts übergeben werden
        > Default-Werte sind in der "defines.h" zu finden
            void setup() {
            input_config config1;
            stage1 = new input_stage(config1);

        > Nun können laufend die Messungen gestartet werden
            void loop() {
                stage1->capture_data();
                ....
                .... }
        
        > Die zuletzt gemessenen Daten werden als Datenstruktor mit "get_data" zurückgegeben
        > Beispiel: Folgende Anweisung gibt die aktuell gemessene Stromstärke in mA aus
            Serial.println(stage1->get_data().adc1_data_ma);
        
        > Hier sollte auch der "timestamp" der Daten beachtet werden um ältere Daten zu verwerfen - struct wird nicht zurückgesetzt
        
        > Die aktuelle Messung wird mit "stop_capture" beendet
        > Kann aber jederzeit mit "stage1->capture_data()" wieder fortgesetzt werden

        > Soll die eingestellte Strombegrenzung geändert werden, geschieht dies mit der Funktion "change_current_limit()"
        > Beispiel: Strombegrenzung soll auf 125mA gändert werden
            stage1->change_current_limit(125.0f);

        > Ob und wann die Strombegrenzung ausgelöst hat lässt sich über die Statusvariablen herausfinden
            if(stage1->get_state().output_disabled)
            { Serial.println("Strom zu hoch! Ausgang abgeschalten!"); }

        > Was jetzt genau im Falle einer Überschreitung geschieht, muss noch besser implementiert werden
        > Machen dann Patrick und ich wenn das Gehäuse etc. fertig ist.
*/

struct input_config{
    /*
        Hier befinden sich alle Parameter die zur Konfiguration der Eingangsschaltung benötigt werden
        Braucht eigentl. nur einmal zum Start des Geräts beim Initialisieren der Klasse "Input-Stage" übergeben werden
        Variablen werden mit den Default-Werten aus der "defines.h" über den Konstruktur initialisiert
    */

    input_config();

    float current_limit;                    //Eingestellte Strombegrenzung in "mA" - kann später noch über "change_current_limit" geändert werden
    float power_limit;                      //Gewünschte   Leistungsbegrenzung in "WATT"

    unsigned long delay;                    //Gewünschte Verzögerung bis die Sicherung bei einer Überschreitung auslöst (MILLISEKUNDEN)

    bool motor_mode;                        //Ist der Motor-Modus beim Starten aktiv?

    ADS1115_CONV_RATE sample_rate_adc1;     //Samplerate vom ADC_1 - Werte aus der ADS1115-Library beachten
    ADS1115_CONV_RATE sample_rate_adc2;     //Samplerate vom ADC_2 - Werte aus der ADS1115-Library beachten

    float op_gain;                          //Konstante des Gains der OP-Amp-Vorverstärkung
    float shunt_value;                      //Widerstandswert des Shunts in mOhm
    float voltage_divider;                  //Spannungsteilerwert für den ADC2

    float level_low, level_high;            //Umschaltpegel für die beiden Messbereiche (Direkt vs OpAmp)

    int adc1_address, adc2_address;         //I2C-Adressen der Beiden ADCs
};

struct input_state{

    /*
        Hier befinden sich alle Zustandsvariablen über die laufende Messung
    */

    input_state();

    int           measure_mode;          //Aktuell verwendeter Messmodus für den ADC1
    bool          running_measurement;   //Laufen gerade Messungen?

    unsigned long timestamp_begin;       //Zeitstempel der bei der ersten Messung der Messreihe aufgenommen wird
    unsigned long timestamp_output;      //Wann wurde der Ausgang abgeschalten?
    unsigned long timestamp_overshoot;   //Wann trat der erste Overshoot auf? Wird wieder zurückgesetzt wenn wir uns im gültigen Bereich bewegen

    int           state_output;          //Zustand in dem sich der Ausgang (Relais) gerade befinden
};

struct input_data{
    
    input_data();

    int   adc1_mode;                 //Messmodus, bei dem die jeweiligen Daten aufgenommen worden
    float adc1_data_mv;              //Daten vom ADC1 schon aufbereitet in der Einheit Millivolt
    float adc1_data_a;
    float adc1_data_ma;              //Daten vom ADC1 schon aufbereitet in der Einheit Milliampere

    unsigned long adc1_num_samples;
    unsigned long adc1_timestamp;    //Zeitstempel der aufgenommenen Daten, um mögliche Berechnungen (Ableitungen etc.) durchzuführen

    float adc2_data_mv;              //Daten vom ADC2 schon aufbereitet in der Einheit Millivolt 
    float adc2_data_v;               //Daten vom ADC2 schon aufbereitet in der Einheit Volt
    unsigned long adc2_num_samples;
    unsigned long adc2_timestamp;    //Zeitstempel der aufgenommenen Daten, um mögliche Berechnungen (Ableitungen etc.) durchzuführen

    float calculated_power;
  };

struct input_errors {
    bool adc1_error;        //Fehler beim Initialisieren ADC1
    bool adc2_error;        //Fehler beim Initialisieren ADC1
    bool range_error_adc1;  //Ungültige Werte beim Einlesen der Daten ADC1 aufgetreten (z.B. negative Spannung)
    bool range_error_adc2;  //Ungültige Werte beim Einlesen der Daten ADC2 aufgetreten (z.B. negative Spannung)
};

class input_stage{

    public:
        input_stage(input_config&);

    public:
        void capture_data();    //Messungen werden gestartet - Stromüberwachung aktiv
        void stop_capture();    //Aktuell laufende Messungen werden eingestellt

    public:
        const input_data& get_data() {return data;}         //Gibt das aktuell ausgelesene Datenpaket zurück
        const input_state& get_state() {return state;}      //Gibt die aktuellen Zustandsvariablen der laufenden Messung zurück
        const input_errors& get_errors() {return errors;}   //Gibt die aktuell aufgetretenen Fehler zurück

        const float get_current_limit() { return config.current_limit; }     //Gibt die eingestellte Stromgrenze zurück
        const float get_power_limit()   { return config.power_limit; }       //Gibt die Leistungsgrenze in (WATT) zurück
        const float get_delay()         { return config.delay; }             //Gibt die gewünschte Verzöerung in (Sekunden) zurück
        const bool  get_motor_mode()    { return config.motor_mode; }        //Gibt zurück ob der Motor-Modus aktiviert ist

        const ADS1115_CONV_RATE get_sample_rate_adc1() { return config.sample_rate_adc1; }
        const ADS1115_CONV_RATE get_sample_rate_adc2() { return config.sample_rate_adc2; }

    public:
        void set_current_limit(float limit) {config.current_limit = limit; } //Strombegrenzung wird neu eingestellt
        void set_power_limit(float limit) {config.power_limit = limit; }     //Leistungsbegrenzung wird neu eingestellt
        void set_delay(float time) { config.delay = time; }                  //Setzt den Motor-Modus
        void set_motor_mode(bool yn) { config.motor_mode = yn; }             //Setzt die Verzögerung in (Sekunden) bis die Sicherung auslöst

    private:
        void start_capture();

        void check_limits();    //Prüft ob eine Strom/Leistungsüberschreitung eingetreten ist und reagiert entsprechend
        void check_range();     //Prüft mögliche Messbereichsüberschreitungen und schaltet zwischen Direct/Opamp um

        void read_adc1();       //Liest einkommende Daten vom ADC #1 aus
        void read_adc2();       //Liest einkommende Daten vom ADC #1 aus

        //Beide Funktionen sind ursprünglich nur zum Debuggen geschrieben worden, können allerdings auch
        //bei Bedarf umfunktioniert werden
        void debug_overcurrent_msg();    //gibt eine Statusnachricht beim Überschreiten des maximalen Stroms aus
        void debug_display_data();       //gibt die gesampelten Daten der beiden ADCs am OLED-Display von Michale aus ##NICHT VERWENDEN##  
        
        static void conversion_rdy_adc1() { data_rdy_adc1=true; asm("WDR"); }
        static void conversion_rdy_adc2() { data_rdy_adc2=true; }

    private:
        input_config config;
        input_state  state;
        input_data data;

        input_errors errors;

        static volatile bool data_rdy_adc1, data_rdy_adc2;

    private:
        ADS1115_WE adc1;
        ADS1115_WE adc2;

};

#endif