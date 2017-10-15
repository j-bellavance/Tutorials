/*
 BlinkWithDelayOneLed.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

#define LED_PIN 13

void blink(byte ledPin, int interval) {
  digitalWrite(ledPin, HIGH);
  delay(interval);
  digitalWrite(ledPin, LOW);
  delay(interval);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  blink(LED_PIN, 1000);
  //And do something else
}

