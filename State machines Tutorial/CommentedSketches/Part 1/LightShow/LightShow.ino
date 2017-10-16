/*
 LightShow.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 1)

  Changes from the previous sketch are shown with <<<<<

*/

//We use tables for the attributes of each LED

enum LedStates{ON, OFF};                                      //The two possible states for an LED
LedStates ledState[8] = {ON, ON, ON, ON, ON, ON, ON, ON};     //<<<<<All LEDs start states are ON
byte ledPin[8] = {2, 3, 4, 5, 6, 7, 8, 9};                    //<<<<<The pins for the LEDs (Don't forget resistors for all LEDs)
int delayOff[8] = {5240, 2560, 1280, 640, 320, 160, 80, 40};  //<<<<<The time in milliseconds that each LED will stay off
int delayOn[8] =  {2560, 1280,  640, 320, 160,  80, 40, 20};  //<<<<<The time in milliseconds that each LED will stay on
unsigned long chrono[8];

void blinkMachine(byte i) {                         //<<<<<We added a parameter: the index for the LED [0..7]
  switch (ledState[i]) {                              //<<<<<Use ledState of LED i
    case OFF: {                                         //state is OFF
      if (millis() - chrono[i] >= delayOff[i]) {          //<<<<<Use chrono and delayOff of LED i
        chrono[i] = millis();                               //<<<<<Reset chrono of LED i
        digitalWrite(ledPin[i], HIGH);                      //<<<<<Write HIGH to LED i 
        ledState[i] = ON;                                   //<<<<<State of LED i is now ON
      }
      break;                                              //Get out of switch
    }
    case ON: {                                          //state is ON
      if (millis() - chrono[i] >= delayOn[i]) {           //<<<<<Use chrono and delayOff of LED i
        chrono[i] = millis();                               //<<<<<Reset chrono of LED i
        digitalWrite(ledPin[i], LOW);                       //<<<<<Write LOW to LED i 
        ledState[i] = OFF;                                   //<<<<<State of LED i is now OFF
      }
      break;                                           //Get out of switch
    }
  }
}

void setup() {
  for (byte i = 0 ; i < 8 ; i++) {      //<<<<<For all leads [0..7]
   pinMode(ledPin[i], OUTPUT);            //<<<<<The pin mode of LED i is OUTPUT
   digitalWrite(ledPin[i], HIGH); }       //<<<<<Bring the pin of LED i HIGH (We said that Start state of all LEDs is ON on line 17)
}

void loop() {
  for (int i = 0 ; i < 8 ; i++)      //<<<<<For all leads [0..7] 
    blinkMachine(i);                   //<<<<<Run the machine with LED i
}

