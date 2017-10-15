/*
 LightShow.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

enum LedStates{ON, OFF};
LedStates ledState[8] = {ON, ON, ON, ON, ON, ON, ON, ON};
byte ledPin[8] = {2, 3, 4, 5, 6, 7, 8, 9};
int delayOff[8] = {1000, 900, 800, 700, 600, 500, 400, 300};
int delayOn[8] = {500, 450, 400, 350, 300, 250, 200, 150};
unsigned long chrono[8];

void blinkMachine(byte i) {
  switch (ledState[i]) {
    case OFF: { 
      if (millis() - chrono[i] >= delayOff[i]) {
        chrono[i] = millis();
        digitalWrite(ledPin[i], HIGH);
        ledState[i] = ON;
      }
      break;
    }
    case ON: { 
      if (millis() - chrono[i] >= delayOn[i]) {
        chrono[i] = millis();
        digitalWrite(ledPin[i], LOW);
        ledState[i] = OFF;
      }
    break;
    }
  }
}

void setup() {
  for (byte i = 0 ; i < 8 ; i++) { 
   pinMode(ledPin[i], OUTPUT); 
   digitalWrite(ledPin[i], HIGH); }
}

void loop() {
  for (int i = 0 ; i < 8 ; i++) blinkMachine(i);
}

