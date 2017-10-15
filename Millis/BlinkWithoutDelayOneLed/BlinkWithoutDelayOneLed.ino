/*
 BlinkWithoutDelayOneLed.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

const int ledPin = 13;
int ledState = 0;
unsigned long previousMillis = 0;
unsigned long interval = 1000;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) ledState = HIGH;
    else                 ledState = LOW;
    digitalWrite(ledPin, ledState);
  }
}

