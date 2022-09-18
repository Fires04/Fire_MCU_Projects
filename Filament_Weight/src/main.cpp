#include <Arduino.h>
#include "HX711.h"
#include<NoDelay.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
HX711 scale;
float offset = 493.45;
float readedWeight = 0;

//button
const int BUTTON_LEFT = 2;
const int BUTTON_RIGHT = 8;
ezButton buttonLeft(BUTTON_LEFT);
ezButton buttonRight(BUTTON_RIGHT);

//display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//NoDelay
void readScale();
void updateDisplay();
noDelay NScaleRead(2000, readScale);
//noDelay NUpdateDisplay(1000, updateDisplay);

//Fillaments
unsigned int fillamentType = 0;
String filamentsName[4] = {"NebS-PET", "NebS-PLA", "Neb-PET", "Neb-PLA"};
// 1cm3 = 0.4158
// PLA 1.24g/cm3
// PLA lenght = (weight / 1.24 ) * 0.4158 = weight * 0.335323
// PETG lenght = (weight / 1.23 ) * 0.4158 = weight * 0.338049
float fillamentsGramToLength[4] = {0.338049,0.335323, 0.338049,0.335323};
float fillamentsSpoolWeight[4] = {278,278,0,0};
float fillamentLength = 0;

void updateDisplay(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(readedWeight,1);
    lcd.print("g");
    lcd.setCursor(8,0);
    lcd.print(filamentsName[fillamentType]);

    lcd.setCursor(0,1);
    lcd.print(readedWeight-fillamentsSpoolWeight[fillamentType],1);
    lcd.print("g");

    lcd.setCursor(8,1);
    lcd.print(fillamentLength);
    lcd.print("m");
}

void readScale() {
  //scale.power_up();
  readedWeight = scale.get_units(10)-offset;
  fillamentLength = (readedWeight-fillamentsSpoolWeight[fillamentType])*fillamentsGramToLength[fillamentType];

  Serial.print("Weight: ");
  Serial.print(readedWeight,1);
  Serial.print("\t|\tType: ");
  Serial.print(fillamentType,1);
  Serial.print("\t|\tName: ");
  Serial.print(filamentsName[fillamentType]);
  Serial.print("\t|\tLength: ");
  Serial.println(fillamentLength,1);

  updateDisplay();
  //Serial.print("one reading:\t");
  //Serial.print(scale.get_units(), 1);
  //Serial.print("\t| average:\t");
  //Serial.println(scale.get_units(10), 1);
  //scale.power_down();			        // put the ADC in sleep mode
}

void routineButton() {
    if(buttonRight.getState()==0 && buttonLeft.getState()==0){
      Serial.println("SCALE TARE");
      scale.tare(20);
      offset = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("----  TARE  ----");
      return;
    }
    if (buttonLeft.isPressed()) {
        Serial.println("Button left pressed");
        if(fillamentType > 0){
          fillamentType--;
        }
        updateDisplay();
    }
    if (buttonRight.isPressed()) {
        Serial.println("Button right pressed");
        if(fillamentType < 4){
          fillamentType++;
        }
        updateDisplay();
    }

}

void setup() {
  Serial.begin(115200);
  Serial.println("--- Start ---");
  
  lcd.init();
  lcd.backlight();


  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Before setting up the scale:");
  scale.set_scale(402.f);
  //scale.tare();
  Serial.println("After setting up the scale");

}

void loop() {
  NScaleRead.update();
  //NUpdateDisplay.update();
  buttonLeft.loop();
  buttonRight.loop();
  routineButton();
}

