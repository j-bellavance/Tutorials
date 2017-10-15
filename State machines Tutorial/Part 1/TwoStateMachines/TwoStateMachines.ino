/*
 TwoStateMachines.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/


enum LedStates{ON, OFF};
LedStates ledState = ON;
byte ledPin = 13;

enum SwitchStates {IS_OPEN, IS_RISING, IS_CLOSED, IS_FALLING};
SwitchStates switchState = IS_OPEN;
byte switchPin = 10;

enum SwitchModes {PULLUP, PULLDOWN};
SwitchModes switchMode = PULLUP;

void toggleMachine() {
  switch (ledState) {
    case OFF: { digitalWrite(ledPin, HIGH); ledState = ON;  break; }
    case ON : { digitalWrite(ledPin, LOW);  ledState = OFF; break; }
  }
}

void switchMachine() {
  byte pinIs = digitalRead(switchPin);
  if (switchMode == PULLUP) pinIs = !pinIs;
  switch (switchState) {
    case IS_OPEN:    { if(pinIs == HIGH) switchState = IS_RISING;  break; }
    case IS_RISING:  {                   switchState = IS_CLOSED;  break; }
    case IS_CLOSED:  { if(pinIs == LOW)  switchState = IS_FALLING; break; }
    case IS_FALLING: { toggleMachine();  switchState = IS_OPEN;    break; }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  if (switchMode == PULLDOWN) pinMode(switchPin, INPUT);
  else                        pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
  switchMachine();
}

