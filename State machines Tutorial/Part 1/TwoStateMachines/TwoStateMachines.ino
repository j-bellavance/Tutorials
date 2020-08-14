/*
 TwoStateMachines.ino
 Author: Jacques Bellavance
 Minor Fixes: Mikel Diez (Arraiz)
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/
#define WANTED_DELAY 2000
enum LedStates { RESET,
                 ON,
                 OFF };
LedStates ledState = OFF;
byte ledPin = 13;

enum SwitchStates { IS_OPEN,
                    IS_RISING,
                    IS_CLOSED,
                    IS_FALLING };
SwitchStates switchState = IS_OPEN;
byte switchPin = 2;

enum SwitchModes { PULLUP,
                   PULLDOWN };
SwitchModes switchMode = PULLUP;

unsigned long chrono = millis();

void toggleMachine() {
    chrono = millis();
    Serial.println("Entered the LED");
    switch (ledState) {
        case OFF: {
            Serial.println("OFF STATE");
            while (millis() - chrono < WANTED_DELAY) {
                digitalWrite(LED_BUILTIN, HIGH);
                Serial.print("Elapsed time:");
                Serial.println(millis() - chrono);
            }
            chrono = millis();
            ledState = ON;
        }
        case ON: {
            Serial.println("ON STATE");
            while (millis() - chrono < WANTED_DELAY) {
                digitalWrite(LED_BUILTIN, LOW);
                Serial.print("Elapsed time : ");
                Serial.println(millis() - chrono);
            }
            chrono = millis();
            ledState = OFF;
        }
    }
}

void switchMachine() {
    byte pinIs = digitalRead(switchPin);
    if (switchMode == PULLUP) pinIs = !pinIs;
    switch (switchState) {
        case IS_OPEN: {
            if (pinIs == HIGH) switchState = IS_RISING;
            break;
        }
        case IS_RISING: {
            switchState = IS_CLOSED;
            break;
        }
        case IS_CLOSED: {
            if (pinIs == LOW) switchState = IS_FALLING;
            break;
        }
        case IS_FALLING: {
            toggleMachine();
            switchState = IS_OPEN;
            break;
        }
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    if (switchMode == PULLDOWN)
        pinMode(switchPin, INPUT);
    else
        pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
    switchMachine();
}
