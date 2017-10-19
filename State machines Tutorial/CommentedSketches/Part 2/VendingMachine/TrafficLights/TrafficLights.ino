/*
 TrafficLights.ino
 Author: Jacques Bellavance
 Released: October 18, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 2)
*/
//The switch for pedestrians====================================================================================================
#include <EdgeDebounceLite.h>    //Setup debounce
EdgeDebounceLite debounce;

bool pedestrians = false;   //Flag to remember that a pedestrian pressed on the switch
byte switchPin = 10;        //Switch pin

enum SwitchStates {IS_OPEN, IS_RISING, IS_CLOSED, IS_FALLING};   //See tutorial part 1
SwitchStates switchState = IS_OPEN;    

enum SwitchModes {PULLUP, PULLDOWN};  //See tutorial part 1
SwitchModes switchMode = PULLUP;

void readSwitch() {
  byte pinIs = debounce.pin(switchPin);          //Debounce the pin
  if (switchMode == PULLUP) pinIs = !pinIs;      //Reverse the read if Pullup
  switch (switchState) {                         //Depending on state
    case IS_OPEN: {                                //State is IS_OPEN (was LOW)
       if(pinIs == HIGH)                             //If pin is HIGH
          switchState = IS_RISING;                     //Change state to IS_RISING 
       break;                                        //Get out of switch
    }
    case IS_RISING: {                              //State is IS_RISING 
      switchState = IS_CLOSED;                       //Change state to IS_CLOSED (Not rising anymore)
      break;                                         //Get out of switch
    }
    case IS_CLOSED: {                              //State is IS_CLOSED (was HIGH)
      if(pinIs == LOW)                               //If pin is LOW
        switchState = IS_FALLING;                      //Change state to IS_FALLING
      break;                                         //Get out of switch
    }
    case IS_FALLING:{                              //State is IS_FALLING
      pedestrians = true;                            //Set the pedestrian flag as true (The switch has been clicked)
      switchState = IS_OPEN;                         //Change state to IS_OPEN (Not falling anymore)
      break;                                         //Get out of switch
    }
  }
}

//The LEDs====================================================================================
#define MAIN_G_PIN 2    //Main Street green LED pin
#define MAIN_Y_PIN 3    //Main Street yellow LED pin
#define MAIN_R_PIN 4    //Main Street red LED pin
#define PINE_G_PIN 5    //Pine Street green LED pin
#define PINE_Y_PIN 6    //Pine Street yellow LED pin
#define PINE_R_PIN 7    //Pine Street red LED pin
#define TURN_PIN   8    //Turn left LED pin
#define WALK_PIN   9    //Pedestrian LED pin

#define BLINK_SPEED 250        //Blink every 250 milliseconds (4 times per seconds)
unsigned long blinkChrono;     //We will use the Blink Without Delay algorithm (See part 1)

void blink(byte ID) {
  if (millis() - blinkChrono >= BLINK_SPEED) {         //If time has elapsed
    blinkChrono = millis();                              //Reset chrono
    digitalWrite(ID, !digitalRead(ID));                  //Change pin's value
  }
}

//The traffic lights state machine==================================================================================================
//Tee the part 2 of the tutorial for the Traffic light State machine
enum TrafficLights { MAIN_G, MAIN_Y, MAIN_R, PINE_G, PINE_Y, PINE_R,
                     TURN_G, TURN_Y, TURN_R, WALK_G, WALK_Y, WALK_R};
TrafficLights statusTL = WALK_R;

unsigned long chrono;   //Light changes are triggered by time lapses

void trafficLights() {
  switch(statusTL) {                            //Depending on status  
    case MAIN_G: {                                //Main Street green light
      if (millis() - chrono >= 20000) {           //If 20 seconds passed  
        chrono = millis();                          //Reset Chronometer
        digitalWrite(MAIN_G_PIN,  LOW);             //Close Main Street's green light
        digitalWrite(MAIN_Y_PIN, HIGH);             //Light up Main Street's yellow light
        statusTL = MAIN_Y; }                        //Change status to Main Street yellow light
      break;                                      //Get out of switch
    }
    case MAIN_Y: {                                //Main Street yellow light
      if (millis() - chrono >=  3000) {
        chrono = millis(); 
        digitalWrite(MAIN_Y_PIN, LOW);
        digitalWrite(MAIN_R_PIN, HIGH); 
        statusTL = MAIN_R; } 
      break; 
    }
    case MAIN_R: {                                //Main Street red light
      if (millis() - chrono >=  2000) {
        chrono = millis(); 
        digitalWrite(PINE_R_PIN,  LOW);
        digitalWrite(PINE_G_PIN, HIGH); 
        statusTL = PINE_G; } 
      break; 
    }
    case PINE_G: {                                //Pine Street green light 
      if (millis() - chrono >= 10000) {
        chrono = millis(); 
        digitalWrite(PINE_G_PIN,  LOW);
        digitalWrite(PINE_Y_PIN, HIGH); 
        statusTL = PINE_Y; } 
      break; 
    }
    case PINE_Y: {                                //Pine Street yellow light
      if (millis() - chrono >=  3000) {
        chrono = millis(); 
        digitalWrite(PINE_Y_PIN, LOW);
        digitalWrite(PINE_R_PIN, HIGH);
        statusTL = PINE_R; } 
      break; 
    }
    case PINE_R: {                                //Pine Street red light 
      if (millis() - chrono >=  2000) {             //If 2 seconds passed
        chrono = millis();                            //Reset chronometer
        digitalWrite(TURN_PIN, HIGH);                 //Light up turn light
        statusTL = TURN_G; }                          //Change state to turn light green
      break;                                        //Get out of switch 
      }
    case TURN_G: {                                //Turn left green light  
      if (millis() - chrono >= 10000) {             //If 10 seconds passed
        chrono = millis();                            //Reset chronometer
        statusTL = TURN_Y; }                          //Change state to turn left yellow
      break;                                        //Get out of switch
    }
    case TURN_Y: {                               //Turn left yellow light
      blink(TURN_PIN);                             //Blink the turn left light (CALL Blink() EVERY TIME THAT THE STATE IS PROBED)
      if (millis() - chrono >=  3000) {            //If 3 seconds passed
        chrono = millis();                           //Reset chronometer
        digitalWrite(TURN_PIN, LOW);                 //Close Turn left light
        statusTL = TURN_R; }                         //Change state to turn left red
      break;                                       //Get out of switch
    }
    case TURN_R: {                               //Turn left red light
      if (millis() - chrono >=  2000) {            //If 2 seconds passed
        chrono = millis();                           //Reset chronometer
        if (pedestrians) {                           //If the pedestrian switch has been pressed during this cycle
          pedestrians = false;                         //Bring the flag low
          digitalWrite(WALK_PIN, HIGH);                //Light up the Walk light
          statusTL = WALK_G;                           //Change state to walk light green
        }
        else {                                       //If no pedestrian pressed the switch during this cycle
          digitalWrite(MAIN_R_PIN,  LOW);              //Close Main Street's red lighht
          digitalWrite(MAIN_G_PIN, HIGH);              //Light up Main street's green light
          statusTL = MAIN_G;                           //Change state to Main Street green light
        } 
      }
      break; 
    }
    case WALK_G: {                               //Walk light green 
      if (millis() - chrono >=  5000) {
        chrono = millis();
        statusTL = WALK_Y; 
      } 
      break; 
    }
    case WALK_Y: {                                //Walk light yellow
      blink(WALK_PIN); 
      if (millis() - chrono >= 10000) {
        chrono = millis(); 
        digitalWrite(WALK_PIN, LOW);
        statusTL = WALK_R; 
      }
      break; 
    }
    case WALK_R: {                               //Walk light red
      if (millis() - chrono >=  2000) {
        chrono = millis(); 
        digitalWrite(MAIN_R_PIN,  LOW); 
        digitalWrite(MAIN_G_PIN, HIGH); 
        statusTL = MAIN_G; 
      } 
      break; 
    }
  }
}

//Setup and loop==================================
void setup() {
  for (byte i =2 ; i <= 9 ; i++) {       //For all 8 LEDs
    pinMode(i, OUTPUT);                    //Mode is OUTPUT
    digitalWrite(i, LOW);                  //All lights are closed
  }
  pinMode(switchPin, INPUT_PULLUP);      //Init the switch in PULLUP mode
  digitalWrite(MAIN_R_PIN, HIGH);          //Main Street's red light is on
  digitalWrite(PINE_R_PIN, HIGH);          //Main Street's red light is on
  //Serial.begin(9600);                    //Un comment for debug pupose only
}

void loop() {
  readSwitch();          //Read the pedestrian switch
  trafficLights();       //Run the Traffic light state machine
}

//NOTE: We did all this with only 2606 bytes of code and 36 bytes of dynamic memory...





