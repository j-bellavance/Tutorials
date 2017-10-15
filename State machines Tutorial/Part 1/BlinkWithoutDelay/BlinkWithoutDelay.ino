/*
 BlinkWithoutDelay.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

enum LedStates{ON, OFF};
LedStates ledState = ON;
byte ledPin = 13;
int delayOff = 1000;
int delayOn = 500;
unsigned long chrono = millis();

void blinkMachine() {
  switch (ledState) {
    case OFF: {
      if (millis() - chrono >= delayOff) {
        chrono = millis();
        digitalWrite(ledPin, HIGH);
        ledState = ON;
      }
      break;
    }
    case ON: { 
      if (millis() - chrono >= delayOn) {
        chrono = millis();
        digitalWrite(ledPin, LOW);
        ledState = OFF;
      }
      break;
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void loop() {
  blinkMachine();
}

