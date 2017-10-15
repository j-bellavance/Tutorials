/*
 ElevatorStateMachine.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b
           

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 3)
*/
#define FLOOR int((elevation + 45) / 1000)

//Handling Requests
bool requests[8][3] = {false, false, false, false, false, false, false, false,  //drop-off
                       false, false, false, false, false, false, false, false,  //pickup going up
                       false, false, false, false, false, false, false, false}; //pickup going down

enum ElevatorStates { FULL_STOP, ACCELERATE, MOVE_DIRECTION, STOP_DECISION, DECELERATE, OPEN_DOOR, 
                      WAIT_FIVE_SECONDS, CLOSE_DOOR,DOOR_CLOSED, HANDLE_DROPOFFS, HANDLE_PICKUPS};
ElevatorStates elevatorState = FULL_STOP;

unsigned long doorOpenChrono;
int direction = 0; //Going down = -1, Standing still = 0 and going up = 1
int elevation = 0; //We start on the ground floor
int speed = 0;     //We will need it later on

bool readMonitor() {
  static char buf[3];   //A buffer for our commands: 2 characters plus the carriage return
  static byte i = 0;    //In what position the next character will be placed in the buffer
  byte flr;             //Table column index: between 0 and 7
  byte requestType;     //Table row index: 0, 1 or 2

  if (Serial.available()>0){ buf[i] = Serial.read(); i++; }
  if (int(buf[2]) == 13) {
    i = 0; buf[2] = 0;
    switch (buf[0]) {
      case 'C': { requestType = 0; break; }
      case 'U': { requestType = 1; break; }
      case 'D': { requestType = 2; break; }
      default:  { return; }  //Reject if it is not 'C', 'U' or 'D'
    }
    flr = buf[1] - 48;  // '0' is ASCII 48
    if (flr < 0 || flr > 7) return; //Has to be between ground floor and seventh floor
    if (flr == 7 && requestType == 1) return; //Can't go above seventh floor
    if (flr == 0 && requestType == 2) return; //Can't go below ground floor
 //   Serial.print(flr); Serial.println(requestType);
    requests[flr][requestType] = true;
  }
}

bool requestsForSameFloor() {
  byte flr = FLOOR; //Calls are made at floor level
  for (int i = 0 ; i < 2 ; i++) if (requests[flr][i]) return true;
  return false;
}

bool requestsFor(byte direction) {
  for (byte flr = FLOOR + direction ; flr >=0 && flr <= 7 ; flr += direction)
    for (int i = 0 ; i < 3 ; i++) if (requests[flr][i]) return true;
  return false;
}

bool requestsForOtherFloors() {
  if (requestsFor( 1)) { direction =  1; return true; }
  if (requestsFor(-1)) { direction = -1; return true; }
  return false;
}

bool requestsForThisFloor() {
  bool pickupSameDirection = false;
  byte flr = FLOOR; //Calls are made at deceleration elevations.
  if (requests[flr][0] == true) return true;                                        //drop-off
  if (!requestsFor(direction)) return true;
    switch (direction) {
    case  1: { if (requests[flr][1] == true)  pickupSameDirection = true; break; }  //pickup going up
    case -1: { if (requests[flr][2] == true)  pickupSameDirection = true; break; }  //pickup going down
  }
  if (pickupSameDirection) return true;
  else                     return false;
}

bool dropOffInMyDirection() {
  byte current = FLOOR; //Calls are made at floor level
  for (byte flr = current + direction; flr >= 0 && flr <= 7 ; flr += direction) {
    if (requests[flr][0]) return true;
  }
  return false;
}

bool pickupsInMyDirection() {
  byte current = FLOOR; //Calls are made at floor level
  for (byte flr = current + direction; flr >= 0 && flr <= 7 ; flr += direction) {
    if (requests[flr][1]) return true;
    if (requests[flr][2]) return true;
  }
  return false;
}

void eraseRequestsForThisFloor() {
  byte flr = FLOOR;
  for (int i = 0 ; i < 3 ; i++) (requests[flr][i])= false;
}

bool upToSpeed() {
  if (speed == 10) return true;
  else             return false;
}

bool nearNextFloor() {
  if (direction ==  1 && elevation % 1000 == 955 
  ||  direction == -1 && elevation % 1000 ==  45) return true;
  return false;
}

bool stopped() {
  if (speed == 0) return true;
  else            return false;
}

bool moving() {
  if(direction != 0) return true;
  else               return false;
}

bool assessDoorClosed () {
  showRequests();
  Serial.println("Door closed");
  return true;
}

void accelerate() {
  speed++; 
  elevation += speed * direction;
  showRequests();
  Serial.println(elevation);
}

void moveElevator() {
  elevation += speed * direction;
  showRequests();
  Serial.println(elevation);
}

void decelerate() {
  speed--; 
  elevation += speed * direction; 
  showRequests();
  Serial.println(elevation);
}

void openDoor() {
  showRequests();
  Serial.println(F("Door open"));
}

void closeDoor() { 
  showRequests();
  Serial.println(F("Closing door"));
}

void elevator() {
  switch (elevatorState) {
    case FULL_STOP: {
      direction = 0;
      if      (requestsForSameFloor())   { Serial.println(F("Request For Same Floor is true"));  elevatorState = OPEN_DOOR;}
      else if (requestsForOtherFloors()) { Serial.println(F("Request For Other Floor is true")); elevatorState = ACCELERATE; }
      break;
    }
    case ACCELERATE: {
      accelerate();
      if (upToSpeed()) elevatorState = MOVE_DIRECTION;
      break;
    }
    case MOVE_DIRECTION: {
      moveElevator();
      if (nearNextFloor()) elevatorState = STOP_DECISION;
      break;
    }
    case STOP_DECISION: {
      if (requestsForThisFloor()) elevatorState = DECELERATE;
      else                        elevatorState = MOVE_DIRECTION;
      break;
    }
    case DECELERATE: {
      decelerate();
      if (stopped()) elevatorState = OPEN_DOOR;
      break;
    }
    case OPEN_DOOR: {
      openDoor();
      eraseRequestsForThisFloor();
      elevatorState = WAIT_FIVE_SECONDS;
      doorOpenChrono = millis();
      break;
    }
    case WAIT_FIVE_SECONDS: {
      if (millis() - doorOpenChrono > 5000) elevatorState = CLOSE_DOOR;
      break;
    }
    case CLOSE_DOOR: {
      closeDoor();
      if (assessDoorClosed()) elevatorState = DOOR_CLOSED;
      break;
    }
    case DOOR_CLOSED: {
      if (moving()) elevatorState = HANDLE_DROPOFFS;
      else          elevatorState = FULL_STOP;
      break;
    }
    case HANDLE_DROPOFFS: {
      Serial.println(F("Handeling requests for drop-offs"));
      if (dropOffInMyDirection()) elevatorState = ACCELERATE;
      else                        elevatorState = HANDLE_PICKUPS;
      break;
    }
    case HANDLE_PICKUPS: {
      Serial.println(F("Handeling requests for pickups"));
      if (pickupsInMyDirection()) elevatorState = ACCELERATE;
      else                        { Serial.println(F("Full Stop")); elevatorState = FULL_STOP; }
      break;
    }
  }
}

void showRequests() {
  for (byte i = 0 ; i < 8 ; i++) Serial.print(requests[i][0]); Serial.print(':');
  for (byte i = 0 ; i < 8 ; i++) Serial.print(requests[i][1]); Serial.print(':');
  for (byte i = 0 ; i < 8 ; i++) Serial.print(requests[i][2]); Serial.print(':');
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("Full Stop"));
}

void loop() {
  readMonitor();
  elevator();
}




