/*
 BlinkWithDelayTwoLeds.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

#define RED_LED_PIN 8
#define BLUE_LED_PIN 9

void blink(byte ledPin, int interval) {
  digitalWrite(ledPin, HIGH);
  delay(interval);
  digitalWrite(ledPin, LOW);
  delay(interval);
}

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
}

void loop() {
  blink(RED_LED_PIN, 1000);
  blink(BLUE_LED_PIN, 500);
  //And do something else
}

