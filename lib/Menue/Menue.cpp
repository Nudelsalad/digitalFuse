#include "Menue.h"



Menu::Menu(Elegoo_TFTLCD *tft, RotaryEncoder *encoder, Elegoo_GFX_Button *buttons){
    m_tft = tft;
    m_encoder = encoder;
    m_buttons = buttons;
    m_button_main = 0; 
    pos = 0;
    newPos = 0;
    m_xAxisVorher = 25;
    m_xAxisNachher = 0;
    m_yAxisVorherStrom = 215;
    m_yAxisVorherLeistung = 215;
    m_yAxisNachherStrom = 0;
    m_yAxisNachherLeistung = 0;
    m_graphCounter = 0;
    m_timestamp = 0.0;
    Eingabe[3][4] = {0};
    zeile = 0;
    spalte = 0;
    m_access = 1;
    whileSchleife = 0;
}






void Menu::Einstellungen(input_stage * input)
{

  
  
  m_tft->reset();
  uint16_t identifier = 0x9341;
  m_tft->begin(identifier);
  m_tft->setRotation(1);
  m_tft->fillScreen(BLACK);
  


  m_buttons[6].initButton(m_tft, BUTTON_Strom_x, BUTTON_Strom_y, BUTTON_Strom_w, BUTTON_Strom_h, ILI9341_WHITE, BLUE, ILI9341_WHITE, "max Strom", BUTTON_TEXTSIZE);
  m_buttons[6].drawButton();
  m_buttons[7].initButton(m_tft, BUTTON_Strom_x, BUTTON_Strom_y + BUTTON_Abstand_y, BUTTON_Strom_w, BUTTON_Strom_h, ILI9341_WHITE, BLUE, ILI9341_WHITE, "max Leistung", BUTTON_TEXTSIZE);
  m_buttons[7].drawButton();
  m_buttons[8].initButton(m_tft, BUTTON_Strom_x, BUTTON_Strom_y + BUTTON_Abstand_y + BUTTON_Abstand_y - 10, BUTTON_Strom_w, BUTTON_Strom_h - 40, ILI9341_WHITE, BLUE, ILI9341_WHITE, "Traegheit", BUTTON_TEXTSIZE);
  m_buttons[8].drawButton();
  
  
  int drehRichtung = 0;
  zeile = 0;
  spalte = 0; 
  int Access = 1;
  whileSchleife = 1;
  m_tft->drawLine(260 - 15 * spalte ,70 + 80*zeile,275 - 15 * spalte,70 + 80*zeile,RED);
  m_tft->drawLine(260 - 15 * spalte ,71 + 80*zeile,275 - 15 * spalte,71 + 80*zeile,RED);


  //TODO:
  //mit isr backspacebutton pushen loesen!!!

  while (whileSchleife)
  {
    m_encoder->tick(); 
    newPos = m_encoder->getPosition();

    if (pos != newPos) {
      drehRichtung = newPos - pos;
      pos = newPos;

      if(drehRichtung > 0){
        
        
        if(Eingabe[zeile][spalte] >= 9){   //wenn die neun ueberdreht wird bei den ersten 3 Spalten
          if(spalte < 3){

            if(Eingabe[zeile][spalte+1]+1 < 9){
              Eingabe[zeile][spalte+1]++;
              Eingabe[zeile][spalte] = 0;
            }

          }
        }else{

          Eingabe[zeile][spalte]++;

        }


        

      }else{       
        if(Eingabe[zeile][spalte]-1 >= 0){  //drehen in die andere Richtung
          Eingabe[zeile][spalte]--;
        }  
      }
        Ausgabe();      //wenn gedreht wurde wird Bildschirm aktualisiert
    }
    


    if (LOW == digitalRead(ROTARY_ENC_SWITCH) && Access)      //frägt ab ob der encoder gedrückt ist, und ob der Zugang erlaubt ist
    { //das Access ist erforderlich, sonst würde er wenn man einmal drückt hier hundert man reinspringen

      Access = 0;

      if(spalte <3){
        
        Serial.print(Eingabe[zeile][spalte]);
        spalte++;
        
      }else{

        Serial.println(Eingabe[zeile][spalte]);
        
        
        zeile++;
        spalte = 0;

        if(zeile == 1){

          StromArrayToFloat(input);

        }else if(zeile == 2){

          LeistungArrayToFloat(input);

        }else if(zeile == 3){

          TraegheitArrayToFloat(input);
          EEPROM.update(12,1);
          whileSchleife = 0;

        }
        
      }
      
      m_tft->drawLine(260 - 15 * spalte ,70 + 80*zeile,275 - 15 * spalte,70 + 80*zeile,RED);
      m_tft->drawLine(260 - 15 * spalte ,71 + 80*zeile,275 - 15 * spalte,71 + 80*zeile,RED);
      m_tft->drawLine(260 - 15 * (spalte-1) ,70 + 80*zeile,275 - 15 * (spalte-1),70 + 80*zeile,BLACK);
      m_tft->drawLine(260 - 15 * (spalte-1) ,71 + 80*zeile,275 - 15 * (spalte-1),71 + 80*zeile,BLACK);

    }

    else if (HIGH == digitalRead(ROTARY_ENC_SWITCH) && Access == 0) //stellt sicher, dass er nur einaml Druck schreibt
    {
      Access = 1;
    }

}
}


  


void Menu::Ausgabe(){

  EEPROMauslesen();

  for(int zeile2 = 0; zeile2 <= 2; zeile2++){
    for (int spalte2 = 0; spalte2 <=3; spalte2++)
    {

      m_tft->fillRect(265 - (15 * spalte2), 48+ 80*zeile2,15,15,BLACK);
      m_tft->setCursor(265 - (15 * spalte2), 48+ 80*zeile2);
      m_tft->print(Eingabe[zeile2][spalte2]);
      
      
    if(zeile2 == 0 || zeile2 == 2){ //zeichne Punkt da printf(".") zu groß
      m_tft->drawPixel(247, 60 + 80*zeile2,WHITE);
      m_tft->drawPixel(247, 61 + 80*zeile2,WHITE);
      m_tft->drawPixel(246, 60 + 80*zeile2,WHITE);
      m_tft->drawPixel(246, 61 + 80*zeile2,WHITE);
    }
    if(zeile2 == 1){ //zeichne Punkt da printf(".") zu groß
      m_tft->drawPixel(262, 60 + 80*zeile2,WHITE);
      m_tft->drawPixel(262, 61 + 80*zeile2,WHITE);
      m_tft->drawPixel(261, 60 + 80*zeile2,WHITE);
      m_tft->drawPixel(261, 61 + 80*zeile2,WHITE);
    }

      m_tft->setCursor(265, 48 + 80*zeile2);

      if(zeile2 == 0){
        m_tft->print("  A");
      }else if(zeile2 == 1){
        m_tft->print("  W");
      }else if(zeile2 == 2){
        m_tft->print("  s");
      }
  }
  }

}



void Menu::GraphInit(input_stage *input){
    int static counter = 0;
    if(counter == 0){
      m_tft->reset();
      uint16_t identifier = 0x9341;
      m_tft->begin(identifier);
      m_tft->setRotation(1);
      counter++;
    }
    
    m_tft->fillScreen(WHITE);
    Serial.println(input->get_current_limit());
    m_tft->drawLine(25,15,25,215,BLACK);                    //Graph plotten
    m_tft->drawLine(25,215,310,215,BLACK);
    m_tft->drawLine(295,15,295,215,BLACK);
    m_tft->drawLine(25,45,295,45,RED);

    m_tft->drawTriangle(310,215,300,210,300,220,BLACK);
    m_tft->fillTriangle(310,215,300,210,300,220,BLACK);
    m_tft->drawTriangle(20,15,25,5,30,15,BLACK);
    m_tft->fillTriangle(20,15,25,5,30,15,BLACK);
    m_tft->drawTriangle(290,15,300,15,295,10,BLACK);
    m_tft->fillTriangle(290,15,300,15,295,10,BLACK);
    
    m_tft->drawLine(20,45,30,45, BLACK);                    //y Achsenabschnitte zeichnen
    m_tft->drawLine(20,85,30,85, BLACK);
    m_tft->drawLine(20,125,30,125, BLACK);
    m_tft->drawLine(20,165,30,165, BLACK);
    
    m_tft->drawLine(285,45,295,45, BLACK);                    //y Achsenabschnitte sekundaer Achse zeichnen
    m_tft->drawLine(285,85,295,85, BLACK);
    m_tft->drawLine(285,125,295,125, BLACK);
    m_tft->drawLine(285,165,295,165, BLACK);

    int tempPrint = 1;
    for(int i = 0; i<18; i++){                          //x Achsenabschnitte zeichnen
      m_tft->drawLine(40+15*i,210,40+15*i,220, BLACK);

      if(tempPrint){                                     //printet nur jeden zweiten Wert
        m_tft->setCursor(38+15*i,225);
        m_tft->setTextSize(1);
        m_tft->setTextColor(BLACK);
        m_tft->print(i+1);
        tempPrint = 0;
      }else{
        tempPrint = 1;
      }

    }

    EEPROMauslesen();
    StromArrayToFloat(input);
    LeistungArrayToFloat(input);
    
    
    float Abschnitte;
    if(input->get_current_limit() > 1000){
      Abschnitte = input->get_current_limit()/4000;
      m_tft->setCursor(32,15);
      m_tft->print("I/A");
    }else{
      Abschnitte = input->get_current_limit()/4;
      m_tft->setCursor(32,15);
      m_tft->print("I/mA");

    }
      float yAxis = 0.0; 

      for(int i=1; i<=5; i++){
        m_tft->setCursor(3,243-i*40);
        m_tft->setTextSize(1);
        if(yAxis > 10.0 || yAxis == 0.0){
          m_tft->print((int)yAxis);
        }else{
          m_tft->print(yAxis);
        }
        yAxis += Abschnitte;      
      }

    yAxis = 0.0;
    //todo getpowerlimit

    //if(input->get_power_limit() < maxStromspitze/leistungspitze abfangen wenn powerlimit = 0.000W)
    Abschnitte = (int)10;

      for(int i=1; i<=5; i++){
        m_tft->setCursor(300,243-i*40);
        m_tft->setTextSize(1);
        m_tft->print((int)yAxis);
        yAxis += Abschnitte;      
      }

      m_tft->setCursor(300,18);
      m_tft->print("P/W");
      m_tft->setCursor(70, 15);
      m_tft->setTextSize(2);
      m_tft->setTextColor(ILI9341_ORANGE);
      m_tft->print("Strom");
      m_tft->setCursor(170, 15);
      m_tft->setTextColor(ILI9341_GREEN);
      m_tft->print("Leistung");


      m_xAxisVorher = 25;

}


void Menu::menuInit(){

  m_tft->reset();
  uint16_t identifier = 0x9341;
  m_tft->begin(identifier);
  m_tft->setRotation(1);
  m_tft->fillScreen(BLACK);
  m_buttons[0].initButton(m_tft,BUTTON_X,BUTTON_Y,BUTTON_W,BUTTON_H,ILI9341_WHITE,BLUE,ILI9341_WHITE,"Betrieb",BUTTON_TEXTSIZE);
  m_buttons[0].drawButton(true);
  m_button_main = 0;
  m_buttons[1].initButton(m_tft,BUTTON_X + BUTTON_X + BUTTON_SPACING_X,BUTTON_Y,BUTTON_W,BUTTON_H,ILI9341_WHITE,BLUE,ILI9341_WHITE,"Plotter",BUTTON_TEXTSIZE);
  m_buttons[1].drawButton();
  m_buttons[2].initButton(m_tft,BUTTON_X,BUTTON_Y + BUTTON_Y + BUTTON_SPACING_Y,BUTTON_W,BUTTON_H,ILI9341_WHITE,BLUE,ILI9341_WHITE,"Motor",BUTTON_TEXTSIZE);
  m_buttons[2].drawButton();
  m_buttons[3].initButton(m_tft,BUTTON_X + BUTTON_X + BUTTON_SPACING_X,BUTTON_Y + BUTTON_Y + BUTTON_SPACING_Y,BUTTON_W,BUTTON_H,ILI9341_WHITE,BLUE,ILI9341_WHITE,"Settings",BUTTON_TEXTSIZE);
  m_buttons[3].drawButton();
  Serial.print("Buttons initialisiert"); 
}

void Menu::graphProcess(input_stage * input){

  m_timestamp = millis();
  float temp;
  int counter = 1;
  //todo: backspace isr für whileschleife

    while(whileSchleife){

      temp = millis() - m_timestamp;        //die zeit die seit der letzten Messung vergangen ist (ist evtl immer unterschiedlich)
      m_timestamp = millis();
      input->capture_data();
      
      m_xAxisNachher = m_xAxisVorher+((temp /18000.0f)*331.0f);
      m_yAxisNachherLeistung = (215 - 170 * ((input->get_data().calculated_power*10.0f/(40.0f))));
      m_yAxisNachherStrom = (215 - 170 * (input->get_data().adc1_data_ma / input->get_current_limit())); //gemessenen Strom gewichten mit dem maxStrom
      m_yAxisVorherStrom = m_yAxisNachherStrom;
      m_yAxisVorherLeistung = m_yAxisNachherLeistung;
      m_xAxisVorher = m_xAxisNachher;



      Serial.print(input->get_data().calculated_power);
      Serial.println(" W");
      Serial.print(input->get_data().adc1_data_ma);
      Serial.println(" mA");
      Serial.print(input->get_current_limit());
      Serial.println(" mA");

      //TODO: powerlimit verwenden
      Serial.print(temp);
      Serial.println("ms");

      Serial.print(m_xAxisNachher);
      Serial.println(" x-Achsenabschnitt");

      m_tft->drawLine((int)m_xAxisVorher+0.5, (int)m_yAxisVorherStrom + 0.5, (int)m_xAxisNachher+0.5, (int)m_yAxisNachherStrom + 0.5, ILI9341_ORANGE);
      m_tft->drawLine((int)m_xAxisVorher+0.5, (int)m_yAxisVorherStrom-1+0.5, (int)m_xAxisNachher+0.5, (int)m_yAxisNachherStrom + 0.5-1, ILI9341_ORANGE);
      m_tft->drawLine((int)m_xAxisVorher+0.5, (int)m_yAxisVorherLeistung+0.5, (int)m_xAxisNachher+0.5, (int)m_yAxisNachherLeistung + 0.5, ILI9341_GREEN);
      m_tft->drawLine((int)m_xAxisVorher+0.5, (int)m_yAxisVorherLeistung+1+0.5, (int)m_xAxisNachher+0.5, (int)m_yAxisNachherLeistung+1+0.5, ILI9341_GREEN);

          
      if(m_xAxisNachher >= 295){    //graph muss von vorne beginnen->alles loeschen und von vorne plotten
        m_xAxisVorher = 25;
        GraphInit(input);
        m_tft->fillRect(38,225,270,10,WHITE);

        int tempPrint = 1;
        for(int a = counter*18; a< (counter+1) * 18; a++){          
          if(tempPrint){
            m_tft->setCursor(38+15*(a-counter*18),225);               
            m_tft->setTextSize(1);                                    
            m_tft->setTextColor(BLACK);                  
            m_tft->print(a);
            tempPrint = 0;
          }else{
          tempPrint = 1;
        }
        }
      if(counter < 55) counter++;
      else counter = 0;
    }
  }
  input->stop_capture();
  m_xAxisVorher = 25;
}



void Menu::BetriebInit(input_stage *input){

  m_tft->reset();
  uint16_t identifier = 0x9341;
  m_tft->begin(identifier);
  m_tft->setRotation(1);
  m_tft->fillScreen(BLACK);
  m_tft->drawRect(2,5,273,30,BLUE);  
  m_tft->setCursor(5,10);
  m_tft->setTextSize(3);
  m_tft->setTextColor(WHITE);
  m_tft->print("Sicherungsmodus");
  m_tft->setTextSize(2);
  m_tft->setCursor(30,50);
  m_tft->print("Strom:");
  m_tft->setCursor(195,50);
  m_tft->print("Leistung:");
  m_tft->setTextSize(1);
  m_tft->setCursor(25,120);
  m_tft->setTextColor(RED);
  m_tft->print("StromMax: ");

  EEPROMauslesen();
  StromArrayToFloat(input);
  LeistungArrayToFloat(input);
  TraegheitArrayToFloat(input);


  if(input->get_current_limit() >= 1000){
    m_tft->print(input->get_current_limit()/1000);  
    m_tft->print(" A");

  }else{
    m_tft->print(input->get_current_limit());
    m_tft->print(" mA");
  }

  m_tft->setCursor(195,120);
  m_tft->print("LeistungMax:");
  m_tft->print((int)input->get_power_limit());
  m_tft->print(" W"); 
  m_tft->setCursor(25,130);
  m_tft->print("Traegheit:");

  if(input->get_delay() == 0){
    m_tft->print("keine");
  }else if(input->get_delay() < 1000){
    m_tft->print((int)input->get_delay());
    m_tft->print(" ms");
  }else{
    m_tft->print(input->get_delay()/1000);
    m_tft->print(" s");
  }
  
  m_tft->setTextSize(2);
  m_tft->setTextColor(WHITE);
  m_tft->setCursor(25,150);
  m_tft->print("Spannung:");
  


  m_tft->setCursor(200,150);
  m_tft->print("Status:");
  aktualisiereStatus(false);
}

void Menu::BetriebProcess(input_stage *input){
  
  float tempStrom, tempSpannung;

  //TODO:
  //temporaere while Bedingung durch whileschleife ersetzen und mit backspaceISR verknuepfen

  input->capture_data();
  whileSchleife = 1;
  tempStrom = input->get_data().adc1_data_ma;
  tempSpannung = input->get_data().adc2_data_mv;
  aktualisiereDisplay(tempStrom, tempSpannung, input->get_data().calculated_power*1000.0f);


  while(whileSchleife){ 

    input->capture_data();
    Serial.println(input->get_state().state_output);
    if(tempStrom != input->get_data().adc1_data_ma || tempSpannung != input->get_data().adc2_data_mv){

      tempStrom = input->get_data().adc1_data_ma;
      tempSpannung = input->get_data().adc2_data_mv;
      aktualisiereDisplay(tempStrom,tempSpannung,input->get_data().calculated_power*1000.0f); //Textfelder mit aktuellem Wert ueberschreiben

    }       
  }     
  input->stop_capture();
  menuInit();

          /*
          if(stage1->get_state().state_output != OUTPUT_ACTIVE){
            stage1->stop_capture();
            menue.aktualisiereStatus(false);
            while(1) { };
          } 
          */               
        //}
  
}


void Menu::Motor(){

  m_tft->reset();
  uint16_t identifier = 0x9341;
  m_tft->begin(identifier);
  m_tft->setRotation(1);
  m_tft->fillScreen(BLACK);
  m_tft->drawRect(2,5,190,30,BLUE);  
  m_tft->setCursor(5,10);
  m_tft->setTextSize(3);
  m_tft->setTextColor(WHITE);
  m_tft->print("Motormodus");
  m_tft->setTextSize(2);
  m_tft->setCursor(30,50);
  m_tft->print("Strom:");
  m_tft->setCursor(195,50);
  m_tft->print("Leistung:");
  m_tft->setCursor(20,130);
  m_tft->setTextSize(1);
  m_tft->print("INFO: hoher Anlaufstrom wird ignoriert");
  m_tft->setTextSize(2);
  m_tft->setCursor(20,160);
  m_tft->print("Traegheit:");
  m_tft->setCursor(25,190);
  m_tft->print("5 Sek.");
  m_tft->setCursor(180,160);
  m_tft->print("Status");
  m_tft->drawRect(200,190,45,30,GREEN);
  m_tft->fillRect(200,190,45,30,GREEN);
  m_tft->setCursor(201,191);
  m_tft->setTextColor(ILI9341_DARKGREEN);
  m_tft->setTextSize(4);
  m_tft->print("ON");

}

void Menu::aktualisiereDisplay(float Strom, float Spannung, float Leistung){


  m_tft->fillRect(5,80,300,35,BLACK);
  m_tft->setCursor(5,80);
  m_tft->setTextSize(4);
  m_tft->setTextColor(WHITE);
  m_tft->print(Strom/1000);
  m_tft->print("A");

  m_tft->setCursor(25,180);
  m_tft->fillRect(25,180,140,35,BLACK);

  if(Spannung < 1000){

    m_tft->print((int)Spannung);
    m_tft->print("mV");

  }else{

    m_tft->print(Spannung/1000);
    m_tft->print("V");

  }
  
  m_tft->setCursor(180,80);

  if(Leistung > 1){
    
    m_tft->print((int)Leistung);
    m_tft->print("W");

  }else{

    m_tft->print((int)Leistung*1000);
    m_tft->print("mW");


  }

    
}

void Menu::aktualisiereStatus(bool status){
  if(status){
    
    m_tft->fillRect(200,190,70,30,BLACK);
    m_tft->drawRect(205,190,45,30,GREEN);
    m_tft->fillRect(205,190,45,30,GREEN);
    m_tft->setCursor(211,191);
    m_tft->setTextColor(ILI9341_DARKGREEN);
    m_tft->setTextSize(4);
    m_tft->print("ON");

  }else{
  
    m_tft->drawRect(200,190,70,30,RED);
    m_tft->fillRect(200,190,70,30,RED);
    m_tft->setCursor(201,191);
    m_tft->setTextColor(BLACK);
    m_tft->setTextSize(4);
    m_tft->print("OFF");
    
  }

}

void Menu::StromArrayToFloat(input_stage *input){

  float temp = 0;

  for(int i=0; i<4; i++){
    temp += Eingabe[0][i] * pow(10.0f, i) * 10.0f;
  }
  Serial.println(temp);

  if(temp > 15000.0f){
    Eingabe[0][0] = 0;
    Eingabe[0][1] = 0;
    Eingabe[0][2] = 5;
    Eingabe[0][3] = 1;
    Ausgabe();
    temp = 15000.0f;
  }

  for(int i=0; i<4; i++){
    EEPROM.update(i, Eingabe[0][i]);
  }
  input->set_current_limit(temp);

}

void Menu::LeistungArrayToFloat(input_stage *input){

  float temp = 0;
  temp += Eingabe[1][0] * 0.1f;
  temp += Eingabe[1][1] * 1.0f;
  temp += Eingabe[1][2] * 10.0f;
  temp += Eingabe[1][3] * 100.0f;
  Serial.println(temp);
  
  if(temp > 800.0f){
    Eingabe[1][0] = 9;
    Eingabe[1][1] = 9;
    Eingabe[1][2] = 9;
    Eingabe[1][3] = 7;
    Ausgabe();
    temp = 800.0f;
  }

  for(int i=0; i<4; i++){

    EEPROM.update(i+4, Eingabe[1][i]);

  }
  //TODO:
  input->set_power_limit(temp);

}

void Menu::TraegheitArrayToFloat(input_stage *input){
  float temp = 0;
  temp += Eingabe[2][0] * 0.01f;
  temp += Eingabe[2][1] * 0.1f;
  temp += Eingabe[2][2] * 1.0f;
  temp += Eingabe[2][3] * 10.0f;
  Serial.println(temp);
  
  if(temp > 10.0f){
    Eingabe[2][0] = 0;
    Eingabe[2][1] = 0;
    Eingabe[2][2] = 0;
    Eingabe[2][3] = 1;
    Ausgabe();
    temp = 10.0f;
  }
          
  for(int i=0; i<4; i++){

    EEPROM.update(i+8, Eingabe[2][i]);

  }
  //TODO:
  input->set_delay(temp*1000);

}

void Menu::EEPROMauslesen(){
  
  if(EEPROM.read(12)){   //flag ob etwas im EEPROM steht
    int tempCounter = 0;

    for(int zeile3 = 0; zeile3 <= 2; zeile3++){
      for (int spalte3 = 0; spalte3 <= 3; spalte3++){
        Eingabe[zeile3][spalte3] = EEPROM.read(tempCounter);
        Serial.print(Eingabe[zeile3][spalte3]);
        tempCounter++;
      }
      Serial.println("");
    }
    EEPROM.update(12,0);

  }

}