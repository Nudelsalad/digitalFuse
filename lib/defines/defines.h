#ifndef _DEFINES_
#define _DEFINES_

/*
ADC1 gilt hier als der Converter, der die Strom/Spannungsmessung direkt am Shunt durchführt
ADC2 misst die Spannungsversorgung an der Last und wird u.U. für Leistungskalkulationen verwendet
*/

/* 
#####################
WICHTIGE WERTE
#####################
*/
#define DEFAULT_CURRENT_LIMIT 65.0f         //Default-Wert wann die Sicherung auslöst (mA)
#define DEFAULT_POWER_LIMIT   NO_LIMIT      //Default-Wert der maximalen Leistung (WATT) an das Last
#define DEFAULT_DELAY         1000          //Default-Wert der Verzögerung in (Millisekunden); Aus = DELAY_OFF

#define DEFAULT_VOLTAGE_DIVIDER 12.06f      //Wert des Spannungsteilers der die Spannung über die Last in ein sicheres Level für den ADC wandelt

#define DEFAULT_MOTOR_MODE        false      //Ob der Motormodus standardmäßig eingeschaltet ist
#define DEFAULT_MOTOR_MODE_DELAY  10000      //Wieviele Millisekunden sollen im Motormodus nachdem Start der Messung gewartet werden, bis Stromüberschreitungen zählen?
  
#define DEFAULT_OP_GAIN 4.0f                //Gain der OPAMP-Schaltung
#define DEFAULT_SHUNT_VALUE 19.40f          //Widerstandswert des Shunts in MILLI-Ohm

#define DEFAULT_LEVEL_LOW 0.0f              //Untere Grenze kleiner Messbereich      (MILLIVOLT über Shunt)
#define DEFAULT_LEVEL_HIGH 35.0f             //Anfang großer Messbereich durch OPAMP  (MILLIVOLT über Shunt)

#define DEBUG 1                             //Erzeugt diverse Statusmeldungen im Serial-Monitor
#define ADC_VERBOSE 0                    //Nur in Verbindung mit DEBUG==1 - Erzeugt eine kontinuierliche Anzeige der Messergebnisse in verschiedenen Formaten im Serial-Monitor

#define WD_ENABLE 0                         //Gibt an ob der Watchdog auf dem ALTERT-Pin des ADC1 aktiv sein soll
/* 
#####################
#####################
*/

/* 
#####################
Various Defines
#####################
*/
  #define RANGE_LOW  0 //Unterer Messbereich - ADC wird direkt verwendet
  #define RANGE_HIGH 1 //Oberer Messbereich - ADC verwendet einen Vorverstärker (OPAMP)
  
  #define DEFAULT_SAMPLE_RATE_ADC1 ADS1115_8_SPS
  #define DEFAULT_SAMPLE_RATE_ADC2 ADS1115_32_SPS
  
  #define ALARM_PIN_ADC1 18

  #define RELAY_PIN_1 12 
  #define RELAY_PIN_2 13

  #define RELAY_TURNON()  ( PORTB |= 0xC0 )
  #define RELAY_TURNOFF() ( PORTB &= ~0xC0 )

  #define ADC1_I2C_ADDRESS  0x48  //I2C-Adresse vom ADC_BOARD
  #define ADC2_I2C_ADDRESS  0x49  //I2C-Adresse vom ADC_BOARD

  //High-Speed = 400000
  //Low- Speed = 100000
  #define I2C_SPEED 100000

  #define ADC1_RANGE_OPAMP ADS1115_RANGE_0512
  #define ADC1_PINS_OPAMP  ADS1115_COMP_0_GND

  #define ADC1_RANGE_DIRECT ADS1115_RANGE_0256
  #define ADC1_PINS_DIRECT ADS1115_COMP_2_3

  #define ADC2_RANGE_DIRECT ADS1115_RANGE_4096
  #define ADC2_PINS_DIRECT  ADS1115_COMP_2_3

  #define OUTPUT_ACTIVE           0
  #define OUTPUT_DISABLED_POWER   1
  #define OUTPUT_DISABLED_CURRENT 2
  #define OUTPUT_DISABLED_ERROR   99 

  #define DELAY_OFF               0
  #define NO_LIMIT                0
/* 
#####################
#####################
*/


/* 
#####################
UI details 
#####################
*/ 

#define BUTTON_X 82
#define BUTTON_Y 60
#define BUTTON_W 150
#define BUTTON_H 100
#define BUTTON_SPACING_X 75
#define BUTTON_SPACING_Y 60
#define BUTTON_TEXTSIZE 2
#define BUTTON_Strom_x 95 //Position auf der x-Achse +13
#define BUTTON_Strom_y 55 //Position auf der y-Achse +12
#define BUTTON_Strom_w 150
#define BUTTON_Strom_h 70
#define BUTTON_Abstand_y 80
#define BUTTON_TEXTSIZE 2

/* 
#####################
#####################
*/

/* 
#####################
Assign human-readable names to some common 16-bit color values:
#####################
*/

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

/* 
#####################
#####################
*/

/* 
#####################
Color definitions
#####################
*/
#define ILI9341_BLACK       0x0000
#define ILI9341_NAVY        0x000F
#define ILI9341_DARKGREEN   0x03E0
#define ILI9341_DARKCYAN    0x03EF
#define ILI9341_MAROON      0x7800
#define ILI9341_PURPLE      0x780F
#define ILI9341_OLIVE       0x7BE0
#define ILI9341_LIGHTGREY   0xC618
#define ILI9341_DARKGREY    0x7BEF
#define ILI9341_BLUE        0x001F
#define ILI9341_GREEN       0x07E0
#define ILI9341_CYAN        0x07FF
#define ILI9341_RED         0xF800
#define ILI9341_MAGENTA     0xF81F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_ORANGE      0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK        0xF81F

/* 
#####################
#####################
*/

/* 
#####################
fuer Drehimpulsgeber
#####################
*/

#define ROTARY_ENC_SWITCH 2
#define PIN_IN1 23
#define PIN_IN2 22
#define Button_Back 19
/* 
#####################
#####################
*/

/* 
#####################
fuers Display
#####################
*/
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
/* 
#####################
#####################
*/

#endif
