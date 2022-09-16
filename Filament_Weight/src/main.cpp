#include <Arduino.h>
#include "HX711.h"
#include<NoDelay.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
HX711 scale;
int readedWeight = 0;

//button
const int BUTTON_LEFT = 8;
const int BUTTON_RIGHT = 2;
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

void updateDisplay(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Weight: ");
    lcd.print(readedWeight);
    lcd.setCursor(0,1);
    lcd.print("F: ");
    lcd.print(fillamentType);
}

void readScale() {
  //scale.power_up();
  readedWeight = scale.get_units(10);
  Serial.print("Weight: ");
  Serial.println(readedWeight,1);
  updateDisplay();
  //Serial.print("one reading:\t");
  //Serial.print(scale.get_units(), 1);
  //Serial.print("\t| average:\t");
  //Serial.println(scale.get_units(10), 1);
  //scale.power_down();			        // put the ADC in sleep mode
}

void routineButton() {
    if (buttonLeft.isPressed()) {
        Serial.println("Button left pressed");
        if(fillamentType >= 0){
          fillamentType--;
        }
        updateDisplay();
    }
    if (buttonRight.isPressed()) {
        Serial.println("Button right pressed");
        if(fillamentType <= 10){
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
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  //scale.tare();				        // reset the scale to 0
  Serial.println("After setting up the scale");

}

void loop() {
  NScaleRead.update();
  //NUpdateDisplay.update();
  buttonLeft.loop();
  buttonRight.loop();
  routineButton();
}

