/*
 ElevatorStateMachine.ino
 Author: Jacques Bellavance
 Released: October 13, 2017
           under GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007 b
           

 This code is to demonstrate the use of state machines in an Arduino sketch.
 It is fully explained in the tutorial: State machine with Arduino (Part 3)
*/

//There are 1000 elevation units per floor. [0..7000]
//When we want to find on which floor we are, we have to divide by 1000.
//When on deceleration altitude (x955 when going up or x045 when going down) we need to find the value of the nearest floor
//When exactly at floor level, x000 + 45 = x045, dividing it by 1000 yelds x.045 and taking its integer value yelds x
//When going UP, at deceleration point, x955 + 45 = y000 (where y = x+1, the next floor)dividing it by 1000 yelds x.090 and taking its integer value yelds x
//When going down, at deceleration point, x045 + 45 = x090
//Whenever we are at floor level or decelerating, the maco FLOOR will give the nearest floor
#define FLOOR int((elevation + 45) / 1000)

//Handling Requests This table has 8 columns, one for each floor and 3 rows, one for each type of request
bool requests[8][3] = {false, false, false, false, false, false, false, false,  //drop-off
                       false, false, false, false, false, false, false, false,  //pickup going up
                       false, false, false, false, false, false, false, false}; //pickup going down

enum ElevatorStates { FULL_STOP, ACCELERATE, MOVE_DIRECTION, STOP_DECISION, DECELERATE, OPEN_DOOR,  //The 11 possible states of the Elevator state machine
                      WAIT_FIVE_SECONDS, CLOSE_DOOR,DOOR_CLOSED, HANDLE_DROPOFFS, HANDLE_PICKUPS};

ElevatorStates elevatorState = FULL_STOP;  //Start state is FULL_STOP

unsigned long doorOpenChrono;  //To count to 5 seconds to let the door open
int direction = 0;             //Going down = -1, Standing still = 0 and going up = 1
int elevation = 0;             //We start on the ground floor
int speed = 0;                 //We will need it later on

//==========================================================================================================================================
//The commands for all the switches will be typed in the serial monitor.
//The commands are made of three parts:
//- A letter ('C' for the buttons in the cabin, 'U' for the up buttons on each floor and 'D' for the down buttons on each floor
//- A number, from 0 to 7 to specify the floor
//- a Carriage Return to say we are finished
//------------------------------------------------------------------------------------------------------------------------------------------
bool readMonitor() {
  static char buf[3];   //A buffer for our commands: 2 characters plus the carriage return
  static byte i = 0;    //In what position the next character will be placed in the buffer
  byte flr;             //Table column index: between 0 and 7
  byte requestType;     //Table row index: 0, 1 or 2

  if (Serial.available()>0){  //If there is character available from the serial monitor
    buf[i] = Serial.read();     //Place it in the buffer
    i++;                        //Advance the buffer pointer
  }
  if (int(buf[2]) == 13) {    //If it is a Carriage Return (the command is complete)
    i = 0;                      //Set the pointer back to the beginning of the buffer
    buf[2] = 0;                 //Erase the Carriage Return

    switch (buf[0]) {           //Check the first character of the command
      case 'C': {                 //It is 'C'
        requestType = 0;            //The row for this type of requests in the request table is 0 
        break;                      //Get out of switch
      }
      case 'U': {                 //It is 'U'
        requestType = 1;            //The row for this type of requests in the request table is 1  
        break;                      //Get out of switch 
      }
      case 'D': {                 //It is 'D' 
        requestType = 2;            //The row for this type of requests in the request table is 2 
        break;                      //Get out of switch 
      }
      default: {                  //If it is not 'C', 'U' or 'D'                 
         return;                    //Reject the command
      }
    }
    flr = buf[1] - 48;                  //The second character of the command is the floor number '0' is ASCII 48 so '0' - 48 = 0
    if (flr < 0 || flr > 7)             //If it is not between ground floor and seventh floor
      return;                             //Reject the command
    if (flr == 7 && requestType == 1)   //If it is a request to go UP from the seventh floor
      return;                             //Reject the command
    if (flr == 0 && requestType == 2)   //If it is a request to go DOWN from the ground floor
      return;                             //Reject the command
 
    requests[flr][requestType] = true;  //We know the command is valid and we set the value in the request table as true
  }
}//readMonitor----------------------------------------------------------------------------------------------------------------------------

//========================================================================================
//Return true if there are any requests for the same floor as the cabin
//----------------------------------------------------------------------------------------
bool requestsForSameFloor() {
  byte flr = FLOOR;
  for (int i = 0 ; i < 2 ; i++)   //For each request type
    if (requests[flr][i])           //If a request for this floor and this type is true
      return true;                    //A request has been found
  return false;                   //No request found
}//requestsForSameFloor-------------------------------------------------------------------

//===============================================================================================================================
//Return true if there are requests for other floors in the direction that the cabin is going
//Ex.: The cabin is going up and is on the 4th floor, we check on floors 5, 6 and 7
//     The cabin is going down and is on the 4th floor, we check on floors 3, 2, 1 and 0
//The for loop is constructed this way:
//- index initialisation is : current floor + direction (Going up, from 4th floor : 4+1=5) (Going down from 4th floor : 4-1=3)
//- stay in the loop as long as we are between floors 0 and 7
//- increase index acocording to the direction, adding or subtracting 1
//-------------------------------------------------------------------------------------------------------------------------------
bool requestsFor(byte direction) {
  for (byte flr = FLOOR + direction ; flr >=0 && flr <= 7 ; flr += direction)
    for (int i = 0 ; i < 3 ; i++)   //For each request type 
      if (requests[flr][i])            //If a request for this floor and this type is true
        return true;                    //A request has been found
  return false;                   //No request found
}//requestsFor-------------------------------------------------------------------------------------------------------------------

//=========================================================================
//Find if there are any requests on another floor than the cabin's floor
//-------------------------------------------------------------------------
bool requestsForOtherFloors() {
  if (requestsFor(1)) {    //Check going UP
    direction =  1;          //Set direction to UP
    return true; }           //A request has been found
  if (requestsFor(-1)) {   //Check going down
    direction = -1;          //Set direction to DOWN
    return true; }           //A request has been found
  return false;            //No request found
}//requestsForOtherFloors--------------------------------------------------

//=========================================================================================================
//Return true if
//- a drop-off request if found on this floor
//- a pickup request going in the same direction of the cabin is found on this floor
//- a pickup request going in the other direction is found AND it is the last request in this direction
//---------------------------------------------------------------------------------------------------------
bool requestsForThisFloor() {
  bool pickupSameDirection = false;
  byte flr = FLOOR;
  if (requests[flr][0] == true)                   //If a drop-off request has been found
    return true;                                    //Return true
  if (!requestsFor(direction))                    //If there are no more requests in my direction
    return true;                                    //Return true
  switch (direction) {                            //Depending of the direction the cabin is going                   
    case  1: {                                      //Going UP
      if (requests[flr][1] == true)                   //If a request is found going UP
        pickupSameDirection = true;                     //Remember it has been found
      break;                                          //Get out of switch
    }
    case -1: {                                      //Going DOWN
      if (requests[flr][2] == true)                   //If a requets is found going DOWN
         pickupSameDirection = true;                    //Remember it has been found
      break;                                          //Get out of switch 
    }
  }
  if (pickupSameDirection)                          //If a request going in my direction is found
    return true;                                      //Return true
  else                                              //If not
    return false;                                     //Return false
}

//===============================================================================================================================
//Return true if there are requests from the cabin for a drop-off in the direction it is going
//Ex.: The cabin is going up and is on the 4th floor, we check on floors 5, 6 and 7
//     The cabin is going down and is on the 4th floor, we check on floors 3, 2, 1 and 0
//The for loop is constructed this way:
//- index initialisation is : current floor + direction (Going up, from 4th floor : 4+1=5) (Going down from 4th floor : 4-1=3)
//- stay in the loop as long as we are between floors 0 and 7
//- increase index acocording to the direction, adding or subtracting 1
//-------------------------------------------------------------------------------------------------------------------------------
bool dropOffInMyDirection() {
  byte current = FLOOR;
  for (byte flr = current + direction; flr >= 0 && flr <= 7 ; flr += direction) {
    if (requests[flr][0])    //If a drop-off request is found
      return true;             //Return true
  }
  return false;            //Return false
}//dropOFfInMyDirection----------------------------------------------------------------------------------------------------------

//===============================================================================================================================
//Return true if there are requests from the cabin for a pickup in the direction it is going
//Ex.: The cabin is going up and is on the 4th floor, we check on floors 5, 6 and 7
//     The cabin is going down and is on the 4th floor, we check on floors 3, 2, 1 and 0
//The for loop is constructed this way:
//- index initialisation is : current floor + direction (Going up, from 4th floor : 4+1=5) (Going down from 4th floor : 4-1=3)
//- stay in the loop as long as we are between floors 0 and 7
//- increase index acocording to the direction, adding or subtracting 1
//-------------------------------------------------------------------------------------------------------------------------------
bool pickupsInMyDirection() {
  byte current = FLOOR;
  for (byte flr = current + direction; flr >= 0 && flr <= 7 ; flr += direction) {
    if (requests[flr][1])     //If there are requests going UP
      return true;              //Return true
    if (requests[flr][2])     //If there are requests going DOWN
      return true;              //Return true
  }
  return false;             //Return false
}//pickupsInMyDirection----------------------------------------------------------------------------------------------------------

//=============================================================
//Erase all requests for this floor
//-------------------------------------------------------------
void eraseRequestsForThisFloor() {
  byte flr = FLOOR;               //Find the floor number
  for (int i = 0 ; i < 3 ; i++)   //For each type of request
    (requests[flr][i])= false;      //Make it false
}//eraseRequestsForThisFloor-----------------------------------

//=================================================
//The cabin accelerates from 0 units to 10 units
//Return true if the cabin is up to speed
//-------------------------------------------------
bool upToSpeed() {
  if (speed == 10)
    return true;
  else 
    return false;
}

//=========================================================================================================
//This is the deceleration decision
//To stop, the cabin needs 45 units to decelerate to full stop
//Return true if we are going UP and the remainder of the actual elevation divided by a thousand is 955
//         or if we are going DOWN and the remainder of the actual elevation divided by a thousand is 45
//---------------------------------------------------------------------------------------------------------
bool nearNextFloor() {
  if (direction ==  1 && elevation % 1000 == 955 
  ||  direction == -1 && elevation % 1000 ==  45) 
    return true;
  return false;
}//nearNextFloor-------------------------------------------------------------------------------------------

//===========================================
//Returns true if the cabin is stopped
//-------------------------------------------
bool stopped() {
  if (speed == 0) 
    return true;
  else
    return false;
}//stopped-----------------------------------

//=========================================
//Returns true if the direction is not 0
//-----------------------------------------
bool moving() {
  if(direction != 0) 
    return true;
  else
    return false;
}//moving---------------------------------

//================================================
//This could be used to read a proximity switch
//to asses that the door is actually closed.
//------------------------------------------------
bool assessDoorClosed () {
  showRequests();
  Serial.println("Door closed");
  return true;
}//assesDoorClosed--------------------------------

//===================================================
//This could be used to accelerate a stepper motor
//Speed is increased and elevation is recalculated
//---------------------------------------------------
void accelerate() {
  speed++; 
  elevation += speed * direction;
  showRequests();
  Serial.println(elevation);
}//accelerate----------------------------------------

//==================================
//Elevation is recalculated
//----------------------------------
void moveElevator() {
  elevation += speed * direction;
  showRequests();
  Serial.println(elevation);
}//moveElevator---------------------

//===================================================
//This could be used to accelerate a stepper motor
//Speed is decreased and elevation is recalculated
//---------------------------------------------------
void decelerate() {
  speed--; 
  elevation += speed * direction; 
  showRequests();
  Serial.println(elevation);
}//decelerate----------------------------------------

//================================================
//This could be used to actuate a stepper motor
//------------------------------------------------
void openDoor() {
  showRequests();
  Serial.println(F("Door open"));
}//openDoor---------------------------------------

//================================================
//This could be used to actuate a stepper motor
//------------------------------------------------
void closeDoor() { 
  showRequests();
  Serial.println(F("Closing door"));
}//closeDoor--------------------------------------

//===========================================================================================================
//The Elevator state machine
//-----------------------------------------------------------------------------------------------------------
void elevator() {
  switch (elevatorState) {                                        //Depending on the state of the machine
    case FULL_STOP: {                                               //State is FULL_STOP
      direction = 0;                                                  //The cabin is not going anywhere
      if (requestsForSameFloor()) {                                   //If there are requests for the same floor
        Serial.println(F("Request For Same Floor is true"));            //Display
        elevatorState = OPEN_DOOR;}                                     //State is now OPEN_DOOR
      else if (requestsForOtherFloors()) {                            //If there are requests for other floors
        Serial.println(F("Request For Other Floor is true"));           //Display
        elevatorState = ACCELERATE; }                                   //State is now ACCELERATE
      break;                                                          //Get out of switch
    }
    case ACCELERATE: {                                              //State is ACCELERATE
      accelerate();                                                   //Do so
      if (upToSpeed())                                                //If the cabin is up to speed
        elevatorState = MOVE_DIRECTION;                                 //State is now MOVE_DIRECTION
      break;                                                          //Get out of switch
    }
    case MOVE_DIRECTION: {                                          //State is MOVE_DIRECTION
      moveElevator();                                                 //Do so
      if (nearNextFloor())                                            //If we are near the next floor
        elevatorState = STOP_DECISION;                                  //State is now STOP_DECISION
      break;                                                          //Get out of switch
    }
    case STOP_DECISION: {                                           //State is STOP_DECISION
      if (requestsForThisFloor())                                     //If there are requests for this floor
        elevatorState = DECELERATE;                                     //State is now DECELEATE
      else                                                            //If not
        elevatorState = MOVE_DIRECTION;                                 //State is now MOVE_DIRECTION
      break;                                                          //Get out of switch
    }
    case DECELERATE: {                                              //State is DECELERATE
      decelerate();                                                   //Do so
      if (stopped())                                                  //If the cabin is stopped
        elevatorState = OPEN_DOOR;                                      //State is now OPEN_DOOR
      break;                                                          //Get out of switch
    }
    case OPEN_DOOR: {                                               //State is OPEN_DOOR
      openDoor();                                                     //Do so
      eraseRequestsForThisFloor();                                    //Erase all the requests for this floor
      elevatorState = WAIT_FIVE_SECONDS;                              //State is now WAiT_FIVE_SECONDS
      doorOpenChrono = millis();                                      //Reset the door open chronometer
      break;                                                          //Get out of switch
    }
    case WAIT_FIVE_SECONDS: {                                       //State is WAIT_FIVE_SECONDS
      if (millis() - doorOpenChrono > 5000)                           //If 5 seconds have passed
        elevatorState = CLOSE_DOOR;                                     //State is now CLOSE_DOOR
      break;                                                          //Get out of switch
    }
    case CLOSE_DOOR: {                                              //State is CLOSE_DOOR
      closeDoor();                                                    //Do so
      if (assessDoorClosed())                                         //If the door is really closed
        elevatorState = DOOR_CLOSED;                                    //State is now DOOR_CLOSED
      break;                                                          //Get out of switch
    }
    case DOOR_CLOSED: {                                             //State is DOOR_CLOSED
      if (moving())                                                   //If the cabin was moving in a direction
        elevatorState = HANDLE_DROPOFFS;                                //State is now HANDLE_DROPOFFS
      else                                                            //If not
        elevatorState = FULL_STOP;                                      //State is now FULL_STOP
      break;                                                          //Get out of switch
    }
    case HANDLE_DROPOFFS: {                                         //State is HANDLE_DROPPOFS
      Serial.println(F("Handeling requests for drop-offs"));          //Display
      if (dropOffInMyDirection())                                     //If there are any drop-offs in the cabin's direction
        elevatorState = ACCELERATE;                                     //State is now ACCELERATE
      else                                                            //If not
        elevatorState = HANDLE_PICKUPS;                                 //State is now HANDLE_PICKUPS
      break;                                                          //Get out of switch
    }
    case HANDLE_PICKUPS: {                                          //State is HANDLE_PICKUPS
      Serial.println(F("Handeling requests for pickups"));            //Display
      if (pickupsInMyDirection())                                     //If there are pickups in the cabin's direction
        elevatorState = ACCELERATE;                                     //State is now ACCELERATE
      else {                                                          //If not
        Serial.println(F("Full Stop"));                                 //Display next state
        elevatorState = FULL_STOP; }                                    //State is now FULL_STOP
      break;                                                          //Get out of switch
    }
  }
}

//=================================================
//Displays the contents of the requests table
//No requests: 
//00000000:00000000:00000000
//Requests : drop-off on floors 3 and 5, 
//           pickup going up on floor4,
//           pickup going down on floors 6 and 7
//00010100:00001000:00000011
//-------------------------------------------------           
void showRequests() {
  for (byte i = 0 ; i < 8 ; i++) 
    Serial.print(requests[i][0]); 
    Serial.print(':');
  for (byte i = 0 ; i < 8 ; i++) 
    Serial.print(requests[i][1]); 
    Serial.print(':');
  for (byte i = 0 ; i < 8 ; i++) 
    Serial.print(requests[i][2]); 
    Serial.print(':');
}//showRequests------------------------------------

void setup() {
  Serial.begin(9600);              //Fire up the Serial monitor
  Serial.println(F("Full Stop"));  //We start in state FULL_STOP
}

void loop() {
  readMonitor();  //Read the serial monitor
  elevator();     //Run the machine
}




