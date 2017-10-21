/*
 VendingMachine.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

#include <EdgeDebounceLite.h>    //Use EdgeDebounceLite to debounce the pin
EdgeDebounceLite debounce;       //Name it debounce

enum VendingMachineStates {WAIT, TWENTYFIVE, FIFTY, GIVE_CHANGE, DROP_CAN};   //The five possible states of the Vending state machine
VendingMachineStates vmState = WAIT;                                          //Start state is WAIT

enum SwitchStates {IS_OPEN, IS_RISING, IS_CLOSED, IS_FALLING};  //The four possible states of the Switch state machine
SwitchStates switchState[2] = {IS_OPEN, IS_OPEN};               //Both switch's states are IS_OPEN
byte switchPin[2] = {10, 11};                                   //Switches are on pins 10 and 11

enum SwitchModes {PULLUP, PULLDOWN};              //The two possible modes for the switches
SwitchModes switchMode[2] = {PULLUP, PULLUP};     //Both switches are in PULLUP mode

byte ledPin[2] = {3, 2};   //LEDs are on pins 3 and 2

void switchMachine(byte i) {                      //The Switch state machine
  byte pinIs = debounce.pin(switchPin[i]);          //<<<<<Replace digitalRead() with debounce.pin()
  if (switchMode[i] == PULLUP) pinIs = !pinIs;      //Reverse the value read if the switch is in PULLUP mode
  switch (switchState[i]) {                         //Depending of the state
    case IS_OPEN:    {                                //State is IS_OPEN
      if(pinIs == HIGH)                                 //If the pin is HIGH
        switchState[i] = IS_RISING;                       //We just changed form LOW to HIGH: State is now IS_RISING 
      break;                                            //Get out of switch
    }
    case IS_RISING:  {                                //State is IS_RISING
      switchState[i] = IS_CLOSED;                       //It is not rising anymore, State is now IS_CLOSED
      break;                                            //Get out of switch
    }
    case IS_CLOSED:  {                               //State is IS_CLOSED
      if(pinIs == LOW)                                 //If the pin is LOW
        switchState[i] = IS_FALLING;                     //We just changed form HIGH to LOW: State is now IS_FALLING 
      break;                                           //Get out of switch 
    }
    case IS_FALLING: {                               //State is IS_FALLING
      switchState[i] = IS_OPEN;                        //It is not falling anymore, State is now IS_OPEN    
      break;                                           //Get out of switch
    }
  }
}

bool dollarInserted() {                     //Find out if a dollar has been inserted
  switchMachine(0);                           //Read switch 0
  if (switchState[0] == IS_FALLING)           //If it is in the state IS_FALLING
    return true;                                //A dollar has been inserted, return true
  else                                        //If not
    return false;                               //return false
}

bool quarterInserted() {                     //Find out if a quarter has been inserted
  switchMachine(1);                           //Read switch 1
  if (switchState[1] == IS_FALLING)           //If it is in the state IS_FALLING
    return true;                                //A quarter has been inserted, return true
  else                                        //If not
    return false;                               //return false
}

void blinkLed(byte i) {                 //A function to blink an LED with ID i
  digitalWrite(ledPin[i], HIGH);          //Bring pin of LED i HIGH
  delay(50);                              //Wait there for 50 milliseconds so we can see it blink
  digitalWrite(ledPin[i], LOW);           //Bring pin of LED i LOW
}

void giveChange() {    //Simulate giving a quarter back for change
  blinkLed(0);           //Blink the first LED
}

void dropCan() {       //Simulate dropping a botthe of water
  blinkLed(1);           //Blink second LED
}
void vendingMachine() {         //The Vending state machine
  switch (vmState) {              //Depending on the state
    case WAIT: {                    //State is WAIT
      if (dollarInserted())           //If a dollar has been inserted
        vmState = GIVE_CHANGE;          //State is now GIVE_CHANGE
      if (quarterInserted())          //If a quarter has been inserted
        vmState = TWENTYFIVE;           //State is now TWETYFIVE 
      break;                          //Get out of switch
    }
    case TWENTYFIVE: {              //State is TWENTYFIFE
      if (quarterInserted())          //If a quarter has been inserted
         vmState = FIFTY;               //State is now FIFTY
       break;                         //Get out of switch
     }
    case FIFTY: {                   //State is FIFTY
      if (quarterInserted())          //If a quarter has been inserted
        vmState = DROP_CAN;             //State is now DROP_CAN
      break;                          //Get out of switch
    }
    case GIVE_CHANGE: {             //State is GIVE_CHANGE
      giveChange();                   //Give the change
      vmState = DROP_CAN;             //State is now DROP_CAN
      break;                          //Get out of switch 
    }
    case DROP_CAN: {                //State is DROP_CAN
      dropCan();                      //Drop the bottle
      vmState = WAIT;                 //State is now WAIT
      break;                          //Get out of switch  
    }
  }
}

void setup() {
  for (int i = 0 ; i < 2 ; i++)             //For each switch
    pinMode(switchPin[i], INPUT_PULLUP);      //Their modes are INPUT_PULLUP
  for (int i = 0 ; i < 2 ; i++) {           //For aech LED
    pinMode(ledPin[i],    OUTPUT);            //Their mode is OUTPUT
    digitalWrite(ledPin[i], LOW);             //They start LOW as we said on line 18
  }
}
void loop() {
  vendingMachine();  //Run the machine
}

