#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ezButton.h>
#include <BasicStepperDriver.h>

#define MOTOR_STEPS 200
#define MICROSTEPS 16
#define PIN_DIR 3
#define PIN_STEP 4
#define PIN_ENABLE 5
#define ANALOG_PIN A0
#define BUTTON_PIN_DIR 7
#define BUTTON_PIN_EN 8


//dynamic variables
int analogValue = 0;
int analogPreviousValue = 0;
int stepperDir = 1;
int stepperRPM = 1;
int stepperEnabled = 0;
int moveSteps = 0;

BasicStepperDriver stepper(MOTOR_STEPS, PIN_DIR, PIN_STEP, PIN_ENABLE);
ezButton buttonDir(BUTTON_PIN_DIR);
ezButton buttonEn(BUTTON_PIN_EN);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void displayDraw() {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (stepperEnabled) {
        lcd.print("Enabled");
    }
    else {
        lcd.print("Disabled");
    }
    lcd.setCursor(0, 1);
    if (stepperDir == 1) {
        lcd.print("Left");
    }
    else {
        lcd.print("Right");
    }
    lcd.setCursor(8, 1);
    lcd.print("RPM:");
    lcd.print(stepperRPM);
}

void startStepper() {
    stepper.stop();
    moveSteps = 9999999 * stepperDir;
    stepper.startRotate(moveSteps);
}


void routineButton() {
    if (buttonDir.isPressed()) {
        Serial.println("Button dir pressed");
        stepperDir = (stepperDir == 1) ? -1 : 1;
        displayDraw();
        startStepper();
    }
    if (buttonEn.isPressed()) {
        Serial.println("Button en pressed");
        if (stepperEnabled == 0) {
            stepperEnabled = 1;
            stepper.enable();
        }
        else {
            stepperEnabled = 0;
            stepper.disable();
        }
        displayDraw();
    }
}

void routineAnalog() {
    analogValue = analogRead(ANALOG_PIN);
    if (analogValue > analogPreviousValue + 30 || analogValue < analogPreviousValue - 30) {
        stepperRPM = map(analogValue,0,1024,0,600);
        analogPreviousValue = analogValue;
        stepper.setRPM(stepperRPM);
        Serial.println((String)"New RPM: "+stepperRPM);
        startStepper();
        displayDraw();
    }
}

void routineLoop() {
    buttonEn.loop();
    buttonDir.loop();
}

void setup() {
    Serial.begin(9600);

    lcd.init();
    lcd.backlight();

    stepper.setEnableActiveState(LOW);
    stepper.setSpeedProfile(BasicStepperDriver::LINEAR_SPEED,2000,2000);
    stepper.begin(stepperRPM, MICROSTEPS);

    routineAnalog();

    stepper.disable();

    displayDraw();
}

void loop() {
    routineLoop();
    routineAnalog();
    routineButton();

    unsigned wait_time_micros = stepper.nextAction();
}
