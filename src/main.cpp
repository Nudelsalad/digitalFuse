#include <Arduino.h>
#include <Wire.h>
#include <ADS1115_WE.h>
#include "Elegoo_GFX.h"
#include "Elegoo_TFTLCD.h"
#include "RotaryEncoder.h"
#include "Menue.h"
#include "input_stage.h"
#include "defines.h"


/* 
#####################
Objekte
#####################
*/

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Elegoo_GFX_Button buttons[4];
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
Sicherung digiFuse(12.0,0.0,0.0);
input_stage* stage1;
Menu menue(&tft,&encoder,buttons);
int pos = 0;
int newPos = 0;
int temp = 0;
int backSpaceFlag = 0;
int mainrotaryPressFlag = 0;


/* 
#####################
#####################
*/
void backSpaceISR(){

  menue.whileSchleife = 0;

}

void rotaryPress(){

  mainrotaryPressFlag = 1;
  
}


void setup()
{
  Serial.begin(9600);
  Serial.print("TFT size is "); 
  Serial.print(tft.width()); 
  Serial.print("x"); 
  Serial.println(tft.height());
  menue.menuInit();

  pinMode(ROTARY_ENC_SWITCH, INPUT_PULLUP);
  pinMode(Button_Back, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Button_Back), backSpaceISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ROTARY_ENC_SWITCH), rotaryPress, FALLING);

  input_config config1;
  stage1 = new input_stage(config1);
  Serial.println("ADC initialisiert");

  EEPROM.update(12,1);  //flag das eeprom noch nicht ausgelesen wurde

}

 
void loop()
{
  encoder.tick();
  newPos = encoder.getPosition();
  if (pos != newPos) {
   
      Serial.println("gedreht");
      temp = newPos - pos;
  
  if(temp > 0){

    if(menue.m_button_main == 3){
      menue.m_buttons[menue.m_button_main].drawButton(false);
      menue.m_button_main = 0;
      menue.m_buttons[menue.m_button_main].drawButton(true);
    }else{
      menue.m_buttons[menue.m_button_main].drawButton(false);
      menue.m_button_main++;
      menue.m_buttons[menue.m_button_main].drawButton(true);
    }
  }else if(temp < 0){

    if(menue.m_button_main == 0){
      menue.m_buttons[menue.m_button_main].drawButton(false);
      menue.m_button_main = 3;
      menue.m_buttons[menue.m_button_main].drawButton(true);

    }else{
      menue.m_buttons[menue.m_button_main].drawButton(false);
      menue.m_button_main--;
      menue.m_buttons[menue.m_button_main].drawButton(true); 
    }
  }
  
  pos = newPos;

  }


  if(mainrotaryPressFlag){
    Serial.println("gedrueckt");
    menue.whileSchleife = 1;


    switch (menue.m_button_main){
      case 0:
        
        menue.BetriebInit(stage1);
        menue.BetriebProcess(stage1);
        

      break;

      case 1:
        menue.GraphInit(stage1);
        menue.graphProcess(stage1);
        menue.menuInit();
        break;
      case 2:
        menue.Motor();
        while(1){
          stage1->capture_data();
          menue.aktualisiereDisplay(stage1->get_data().adc1_data_a, stage1->get_data().adc2_data_v, stage1->get_data().calculated_power*1000.0f);
        }
        break;
      case 3:
        menue.Einstellungen(stage1); 
        menue.menuInit();
        break;
    }
    
    mainrotaryPressFlag = 0;

  }

}
