#ifndef MENUE_H
#define MENUE_H
#include "Elegoo_TFTLCD.h"
#include "RotaryEncoder.h"
#include "input_stage.h"
#include "EEPROM.h"
#include "defines.h"



class Menu{

public:

    Elegoo_TFTLCD *m_tft;
    RotaryEncoder *m_encoder;
    Elegoo_GFX_Button *m_buttons;
    int m_button_main;
    int pos;
    int newPos;
    int m_xAxisVorher, m_xAxisNachher;
    int m_yAxisVorherStrom = 215, m_yAxisVorherLeistung = 215, m_yAxisNachherStrom, m_yAxisNachherLeistung;
    int m_graphCounter;
    unsigned long m_timestamp;
    uint8_t Eingabe[3][4]; //zuerst Strom, dann Leistung, dann Traegheit
    int zeile;
    int spalte;
    int m_access;
    int whileSchleife;
    
public:
    Menu(Elegoo_TFTLCD *tft, RotaryEncoder *encoder, Elegoo_GFX_Button *buttons);
    void Ausgabe();
    void Einstellungen(input_stage *input);
    void GraphInit(input_stage *input);
    void menuInit();
    void graphProcess(input_stage *input);
    void BetriebInit(input_stage *input);
    void BetriebProcess(input_stage *input);
    void Motor();
    void aktualisiereDisplay(float Strom, float Spannung,float Leistung);
    void aktualisiereStatus(bool status);
    void StromArrayToFloat(input_stage *input);
    void LeistungArrayToFloat(input_stage *input);
    void TraegheitArrayToFloat(input_stage *input);
    void EEPROMauslesen();

    
};


#endif
