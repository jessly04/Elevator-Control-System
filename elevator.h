
#ifndef elevator_H    /* Guard against multiple inclusion */
#define elevator_H

extern volatile unsigned int inputState;

extern volatile unsigned int elevatorINState ; // 0 user floor, 1 direction, destination

extern volatile unsigned int select;

extern volatile unsigned int currFloor; // first floor
extern volatile unsigned int dir; // 10: up, 11: Down 

extern volatile unsigned int elevatorState; // 0 idle, moving, arrived, door open

extern volatile unsigned int destination;

extern volatile unsigned int queLen;

extern volatile unsigned int userFloor;

extern volatile unsigned int holdRequested;

enum DIRECTION {
    up = 10,
    down = 11
};

enum elevator_State {
    idle = 0,
    moving = 1,
    arrived = 2,
    doorOpen = 3,
    holdDoor = 4,
    forceClose = 5,
    trueIdle = 6
};
enum elevatorIn_State {
    userFloorState = 0,
    directionState = 1,
    destinationState = 2
};
enum ELE_ED_PINS {
    GREEN_LED = (1 << 8),
    RED_LED = (1 << 9),
    YELLOW_LED = (1 << 10),
    BUZZER = (1 << 18)
};

void close();
void hold();
void elevator();

#endif 

