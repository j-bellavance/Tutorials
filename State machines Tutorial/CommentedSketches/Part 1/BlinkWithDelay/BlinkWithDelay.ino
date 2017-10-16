/*
 BlinkWithDelay.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)

 Changes from the previous sketch are shown with <<<<<

*/

enum LedStates{ON, OFF};    //The two possible states for an LED
LedStates ledState = ON;    //Start state is ON
byte ledPin = 13;           //LED is on pin 13 (onboard LED)

void blinkMachine() {               //The Blink state machine
  switch (ledState) {                //Depending of the state
    case OFF: {                        //State is OFF
      delay(1000);                       //<<<<<Wait there for 1000 milliseconds
      digitalWrite(ledPin, HIGH);        //Bring LED pin HIGH
      ledState = ON;                     //State is now ON
      break;                             //Get out of switch
    }
    case ON : {                        //State is OFF
      delay(500);                        //<<<<<Wait there for 500 milliseconds
      digitalWrite(ledPin, LOW);         //Bring LED pin LOW
      ledState = OFF;                    //State is now OFF
      break;                             //Get out of switch
    }
  }
} 

void setup() {
  pinMode(ledPin, OUTPUT);     //LED pin mode is OUTPUT
  digitalWrite(ledPin, HIGH);  //Bring it HIGH (We said that Start state is ON on line 15)
}

void loop() {
  toggleMachine();  //Run the machine
}

