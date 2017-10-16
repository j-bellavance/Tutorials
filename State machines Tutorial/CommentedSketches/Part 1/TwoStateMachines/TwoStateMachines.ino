/*
 TwoStateMachines.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

enum LedStates{ON, OFF};    //The two possible states for an LED
LedStates ledState = ON;    //Start state is ON
byte ledPin = 13;           //LED is on pin 13 (onboard LED)

enum SwitchStates {IS_OPEN, IS_RISING, IS_CLOSED, IS_FALLING};    //The four possible states for a switch
SwitchStates switchState = IS_OPEN;                               //Start state is IS_OPEN
byte switchPin = 10;                                              //The switch is on pin 10

enum SwitchModes {PULLUP, PULLDOWN};    //The two possible modes for a switch
SwitchModes switchMode = PULLUP;        //The switch is in PULLUP mode (one of the switche's pin to pin 10 and the other one to ground)

void toggleMachine() {               //The Toggle state machine         
  switch (ledState) {                  //Depending of the state
    case OFF: {                          //State is OFF
      digitalWrite(ledPin, HIGH);          //Bring LED pin HIGH 
      ledState = ON;                       //State is now ON  
      break;                               //Get out of switch 
    }
    case ON: {                           //State is ON
      digitalWrite(ledPin, LOW);           //Bring LED pin LOW
      ledState = OFF;                      //State is now OFF
      break;                               //Get out of switch
    }
  }
}

void switchMachine() {                        //The Switch state machine
  byte pinIs = digitalRead(switchPin);          //Read the pin
  if (switchMode == PULLUP) pinIs = !pinIs;     //Reverse the value read if the switch is in PULLUP mode
  switch (switchState) {                        //Depending of the state
    case IS_OPEN:    {                            //State is IS_OPEN
      if(pinIs == HIGH)                             //If the pin is HIGH
        switchState = IS_RISING;                      //We just changed form LOW to HIGH: State is now IS_RISING 
      break;                                        //Get out of switch
    }
    case IS_RISING:  {                            //State is IS_RISING
      switchState = IS_CLOSED;                      //It is not rising anymore, State is now IS_CLOSED
      break;                                        //Get out of switch
    }
    case IS_CLOSED:  {                            //State is IS_CLOSED
      if(pinIs == LOW)                              //If the pin is LOW
        switchState = IS_FALLING;                     //We just changed form HIGH to LOW: State is now IS_FALLING 
      break; 
    }
    case IS_FALLING: {                             //State is IS_FALLING
      toggleMachine();                               //Call the Toggle state machine  
      switchState = IS_OPEN;                         //It is not falling anymore, State is now IS_OPEN    
      break;                                         //Get out of switch
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);              //LED pin mode is OUTPUT
  digitalWrite(ledPin, HIGH);           //Bring it HIGH (We said that Start state is ON on line 12)
  if (switchMode == PULLDOWN)           //If the switch is in PULLDOWN mode 
    pinMode(switchPin, INPUT);            //Pin mode is INPUT
  else                                  //If not
    pinMode(switchPin, INPUT_PULLUP);     //Pin mode is INPUT_PULLUP
}

void loop() {
  switchMachine();  //Run the machine
}

