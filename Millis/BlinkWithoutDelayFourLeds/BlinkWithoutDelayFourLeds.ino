/*
 BlinkWithoutDelayFourLeds.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/


byte ledPin[4] = {6, 7, 8, 9};
byte ledState[4] = {LOW, LOW, LOW, LOW};
unsigned long previousMillis[4] = {0, 0, 0, 0};
int interval[4] = {1000, 500, 250, 125};

void blink(byte id) { //id will be in the range 0..3
  if(millis() - previousMillis[id] >= interval[id]) {
    previousMillis[id] = millis();
    if (ledState[id] == LOW) ledState[id] = HIGH;
    else                     ledState[id] = LOW;
    digitalWrite(ledPin[id], ledState[id]);
  }  
}

void setup() {
  for (byte i = 0 ; i < 4 ; i++) pinMode(ledPin[i], OUTPUT);
}

void loop() {
  for (byte i = 0 ; i < 4 ; i++) blink(i);
  //Do other stuff
}

