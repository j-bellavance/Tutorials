/*
 BlinkWithDelay.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)
*/

enum LedStates{ON, OFF};
LedStates ledState = ON;
byte ledPin = 13;

void blinkMachine() {
  switch (ledState) {
    case OFF: {
      delay(1000);                    //A condition to get out of the state
      digitalWrite(ledPin, HIGH);     //Action to take on state change
      ledState = ON;                  //Change state
      break;
    }
    case ON : {
      delay(500);
      digitalWrite(ledPin, LOW);
      ledState = OFF;
      break;
    }
  }
} 

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
}

void loop() {
  toggleMachine();
}

