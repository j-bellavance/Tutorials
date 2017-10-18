/*
 TrafficLights.ino
 Author: Jacques Bellavance
 Released: October 18, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 2)
*/
//The switch for pedestrians===============================================================
#include <EdgeDebounceLite.h>
EdgeDebounceLite debounce;

bool pedestrians = false;
byte switchPin = 10;

enum SwitchStates {IS_OPEN, IS_RISING, IS_CLOSED, IS_FALLING};
SwitchStates switchState = IS_OPEN;

enum SwitchModes {PULLUP, PULLDOWN};
SwitchModes switchMode = PULLUP;

void readSwitch() {
  byte pinIs = debounce.pin(switchPin);
  if (switchMode == PULLUP) pinIs = !pinIs;
  switch (switchState) {
    case IS_OPEN:    { if(pinIs == HIGH)   switchState = IS_RISING;  break; }
    case IS_RISING:  {                     switchState = IS_CLOSED;  break; }
    case IS_CLOSED:  { if(pinIs == LOW)    switchState = IS_FALLING; break; }
    case IS_FALLING: { pedestrians = true; switchState = IS_OPEN;    break; }
  }
}

//The LEDs===============================================================================
#define MAIN_G_PIN 2    //Main Street green LED pin
#define MAIN_Y_PIN 3    //Main Street yellow LED pin
#define MAIN_R_PIN 4    //Main Street red LED pin
#define PINE_G_PIN 5    //Pine Street green LED pin
#define PINE_Y_PIN 6    //Pine Street yellow LED pin
#define PINE_R_PIN 7    //Pine Street red LED pin
#define TURN_PIN   8    //Turn left LED pin
#define WALK_PIN   9    //Pedestrian LED pin

#define BLINK_SPEED 250
unsigned long blinkChrono;

void blink(byte ID) {
  if (millis() - blinkChrono >= BLINK_SPEED) {
    blinkChrono = millis();
    digitalWrite(ID, !digitalRead(ID));
  }
}

//The traffic lights state machine========================================================
enum TrafficLights { MAIN_G, MAIN_Y, MAIN_R, PINE_G, PINE_Y, PINE_R,
                     TURN_G, TURN_Y, TURN_R, WALK_G, WALK_Y, WALK_R};
TrafficLights statusTL = WALK_R;

unsigned long chrono;

void trafficLights() {
  switch(statusTL) {
    case MAIN_G: { if (millis() - chrono >= 20000) { 
                     chrono = millis(); 
                     digitalWrite(MAIN_G_PIN,  LOW);
                     digitalWrite(MAIN_Y_PIN, HIGH); 
                     statusTL = MAIN_Y; } 
                   break; }
    case MAIN_Y: { if (millis() - chrono >=  3000) {
                     chrono = millis(); 
                     digitalWrite(MAIN_Y_PIN, LOW);
                     digitalWrite(MAIN_R_PIN, HIGH); 
                     statusTL = MAIN_R; } 
                   break; }
    case MAIN_R: { if (millis() - chrono >=  2000) {
                     chrono = millis(); 
                     digitalWrite(PINE_R_PIN,  LOW);
                     digitalWrite(PINE_G_PIN, HIGH); 
                     statusTL = PINE_G; } 
                   break; }
    case PINE_G: { if (millis() - chrono >= 10000) {
                     chrono = millis(); 
                     digitalWrite(PINE_G_PIN,  LOW);
                     digitalWrite(PINE_Y_PIN, HIGH); 
                     statusTL = PINE_Y; } 
                   break; }
    case PINE_Y: { if (millis() - chrono >=  3000) {
                      chrono = millis(); 
                      digitalWrite(PINE_Y_PIN, LOW);
                      digitalWrite(PINE_R_PIN, HIGH);
                      statusTL = PINE_R; } 
                   break; }
    case PINE_R: { if (millis() - chrono >=  2000) {
                     chrono = millis();
                     digitalWrite(TURN_PIN, HIGH);
                     statusTL = TURN_G; } 
                   break; }
    case TURN_G: { if (millis() - chrono >= 10000) { 
                     chrono = millis(); 
                     statusTL = TURN_Y; } 
                   break; }
    case TURN_Y: { blink(TURN_PIN);
                   if (millis() - chrono >=  3000) {
                     chrono = millis();
                     digitalWrite(TURN_PIN, LOW); 
                     statusTL = TURN_R; } 
                   break; }
    case TURN_R: { if (millis() - chrono >=  2000) {
                     chrono = millis();
                     if (pedestrians) { 
                        pedestrians = false;
                        digitalWrite(WALK_PIN, HIGH);
                        statusTL = WALK_G; }
                     else { 
                       digitalWrite(MAIN_R_PIN,  LOW); digitalWrite(MAIN_G_PIN, HIGH);
                       statusTL = MAIN_G; } }
                   break; }
    case WALK_G: { if (millis() - chrono >=  5000) {
                      chrono = millis();
                      statusTL = WALK_Y; } 
                    break; }
    case WALK_Y:  { blink(WALK_PIN); 
                    if (millis() - chrono >= 10000) {
                      chrono = millis(); 
                      digitalWrite(WALK_PIN, LOW);
                      statusTL = WALK_R; }
                     break; }
    case WALK_R:  { if (millis() - chrono >=  2000) {
                      chrono = millis(); 
                      digitalWrite(MAIN_R_PIN,  LOW); digitalWrite(MAIN_G_PIN, HIGH); 
                      statusTL = MAIN_G; } 
                   break; }
  }
}

void setup() {
  for (byte i =2 ; i <= 9 ; i++) { pinMode(i, OUTPUT); digitalWrite(i, LOW); }
  pinMode(switchPin, INPUT_PULLUP);
  digitalWrite(MAIN_R_PIN, HIGH);
  digitalWrite(PINE_R_PIN, HIGH);
  Serial.begin(9600);
}

void loop() {
  readSwitch();
  trafficLights();
}
