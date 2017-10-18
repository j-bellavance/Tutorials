/*
 VendingMachine.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 2)
*/

#include <EdgeDebounceLite.h>
EdgeDebounceLite debounce;

enum VendingMachineStates {WAIT, TWENTYFIVE, FIFTY, GIVE_CHANGE, DROP_CAN};
VendingMachineStates vmState = WAIT;

enum SwitchStates {IS_OPEN, IS_RISING, IS_CLOSED, IS_FALLING};
SwitchStates switchState[2] = {IS_OPEN, IS_OPEN};
byte switchPin[2] = {10, 11};

enum SwitchModes {PULLUP, PULLDOWN};
SwitchModes switchMode[2] = {PULLUP, PULLUP};

byte ledPin[2] = {3, 2};

void switchMachine(byte i) {
  byte pinIs = debounce.pin(switchPin[i]);
  if (switchMode[i] == PULLUP) pinIs = !pinIs;
  switch (switchState[i]) {
    case IS_OPEN:    { if(pinIs == HIGH) switchState[i] = IS_RISING;  break; }
    case IS_RISING:  {                   switchState[i] = IS_CLOSED;  break; }
    case IS_CLOSED:  { if(pinIs == LOW)  switchState[i] = IS_FALLING; break; }
    case IS_FALLING: {                   switchState[i] = IS_OPEN;    break; }
  }
}

bool dollarInserted() {
  switchMachine(0);
  if (switchState[0] == IS_FALLING) return true;
  else                              return false;
}

bool quarterInserted() {
  switchMachine(1);
  if (switchState[1] == IS_FALLING) return true;
  else                              return false;
}
void blinkLed(byte i) {
  digitalWrite(ledPin[i], HIGH);
  delay(50);
  digitalWrite(ledPin[i], LOW);
}

void giveChange() {
  blinkLed(0);
}

void dropCan() {
  blinkLed(1);
}
void vendingMachine() {
  switch (vmState) {
    case WAIT:        { if (dollarInserted())  vmState = GIVE_CHANGE;
                        if (quarterInserted()) vmState = TWENTYFIVE;   break; }
    case TWENTYFIVE:  { if (quarterInserted()) vmState = FIFTY;        break; }
    case FIFTY:       { if (quarterInserted()) vmState = DROP_CAN;     break; }
    case GIVE_CHANGE: { giveChange();          vmState = DROP_CAN;     break; }
    case DROP_CAN:    { dropCan();             vmState = WAIT;         break; }
  }
}

void setup() {
  for (int i = 0 ; i < 2 ; i++)   pinMode(switchPin[i], INPUT_PULLUP);
  for (int i = 0 ; i < 2 ; i++) { pinMode(ledPin[i],    OUTPUT)      ; digitalWrite(ledPin[i], LOW); }
}
void loop() {
  vendingMachine();
}

