#include <xc.h>
#include "elevator.h"
#include "keyPad.h"
#include "delay.h"
#include "LCD.h"
#include "timerSetUp.h"


volatile unsigned int que[12] = {};
volatile unsigned int quePointer = 0;
volatile unsigned int frontQuePointer = 0;

volatile unsigned int inputState = 0;

volatile unsigned int elevatorINState = 0; // 0 user floor, 1 direction, destination

volatile unsigned int select;

volatile unsigned int currFloor = 1; // first floor
volatile unsigned int dir = up; // 10: up, 11: Down 

volatile unsigned int elevatorState = idle; // 0 idle, moving, arrived, door open

volatile unsigned int destination = 1;

volatile unsigned int queLen = 0;

volatile unsigned int userFloor = 1;

volatile unsigned int holdRequested = 0;

volatile unsigned int idleCount = 0;

volatile unsigned int userCarry = 0;

unsigned int pickingUp = 0;
unsigned int blinked = 0;

void elevator(){

    if((currFloor == destination) && !queLen){
        PORT_REGS->GROUP[0].PORT_OUTCLR = RED_LED;
        if(userCarry){
            PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;
            delay_ms(100);
            PORT_REGS->GROUP[0].PORT_OUTCLR = GREEN_LED;
            userCarry = 0;
            delay_ms(100);
            PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;
            delay_ms(100);
            PORT_REGS->GROUP[0].PORT_OUTCLR = GREEN_LED;
            delay_ms(100);
            PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;
        }
        else if(!blinked){
            PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;
            delay_ms(400);
            PORT_REGS->GROUP[0].PORT_OUTCLR = GREEN_LED;
            blinked = 1;
            
        }
        
        //PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;

    }
    else if((currFloor == destination) && queLen){
        // assuming the queue isnt empty
        PORT_REGS->GROUP[0].PORT_OUTCLR = RED_LED;
        //PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;

        // it will take the first input which is the users floor
        destination = que[frontQuePointer];
        // if the users floor is higher than the current floor
        // the elevator is on then go up
        if(destination > currFloor){
            dir = up;
            elevatorState = idle;
        }//otherwise down
        else if (destination < currFloor){
            dir = down;
            elevatorState = idle;
        }// on the off case its on the user floor open door
        frontQuePointer++;
        queLen--;
//        else{
//            // turn on green LED
//            PORT_REGS->GROUP[0].PORT_OUTCLR = RED_LED;
//            PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;
//
//            // load the direction and destination
//            frontQuePointer++; // was still on user floor so inc
//            dir = que[frontQuePointer]; // dir
//            frontQuePointer++; 
//            destination = que[frontQuePointer]; //destination
//            frontQuePointer++;
//            elevatorState = idle; // go idle for 2 seconds then begin moving
//            queLen--; // one full instruction loaded dec queue len
//            userCarry = 1;
//        }
        if(pickingUp == 1){
            userCarry = 1;
            pickingUp = 0;
        }
        else{
            pickingUp = 1;
        }

        // once the quePointer hits 12 loop back arround
        if(frontQuePointer == 12){
            frontQuePointer = 0;
        }
        // if que is empty then just idle
        delay_ms(100);
        PORT_REGS->GROUP[0].PORT_OUTCLR = GREEN_LED; // blinks green LED
        blinked = 0;
    }

    
    // if the current floor is not the destination
    // and elevator is moving then increase floor or decrease
    // depending on direction
    if((currFloor != destination) && (elevatorState == moving)){
        if (dir == up){
            currFloor++;
        }
        else{
            currFloor--;
        }
        if(userCarry){
            PORT_REGS->GROUP[0].PORT_OUTCLR = RED_LED;
        }
        else{
            PORT_REGS->GROUP[0].PORT_OUTSET = RED_LED;
        }
        PORT_REGS->GROUP[0].PORT_OUTCLR = GREEN_LED;
    }
    
    LCD_CLEAR();
    LCD_WRITE_STR("Floor:", 6);
    switch(currFloor){
        case 1:
            LCD_WRITE_CHAR('1');
            break;
        case 2:
            LCD_WRITE_CHAR('2');
            break;
        case 3:
            LCD_WRITE_CHAR('3');
            break;
        case 4:
            LCD_WRITE_CHAR('4');
            break;
        case 5:
            LCD_WRITE_CHAR('5');
            break;
        case 6:
            LCD_WRITE_CHAR('6');
            break;
        case 7:
            LCD_WRITE_CHAR('7');
            break;
        case 8:
            LCD_WRITE_CHAR('8');
            break;
        case 9:
            LCD_WRITE_CHAR('9');
            break;
            
    }
    
    // upon arriving at the destination the elevator will check the next item in queue

    
    // if hold for more than 10s the Tc4 will go off yellow LED goes off
    if(elevatorState == forceClose){
        PORT_REGS->GROUP[0].PORT_OUTCLR = GREEN_LED;
        PORT_REGS->GROUP[0].PORT_OUTCLR = RED_LED;
        PORT_REGS->GROUP[0].PORT_OUTSET = YELLOW_LED;
        PORT_REGS->GROUP[0].PORT_OUTSET = BUZZER;
        elevatorState = moving; // close and move
    }
    else{
        PORT_REGS->GROUP[0].PORT_OUTCLR = YELLOW_LED;
        PORT_REGS->GROUP[0].PORT_OUTCLR = BUZZER;
    }
    

    // idle // so waits 2 seconds and then begins to move
    if(!elevatorState){
        //PORT_REGS->GROUP[0].PORT_OUTSET = GREEN_LED;
        if(idleCount > 4){
            elevatorState = moving;
            idleCount = 0;
        }
        idleCount++;
    }
    
//    if(destination != currFloor){        
//        PORT_REGS->GROUP[0].PORT_OUTSET = RED_LED;
//        PORT_REGS->GROUP[0].PORT_OUTCLR = GREEN_LED;
//    }
    
}

void hold(){
    // make elvator idle
    elevatorState = holdDoor;
    Timer4_Init();

    //Enable TC4 start counting to 10s
    TC4_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;
    while (TC4_REGS->COUNT16.TC_STATUS & TC_STATUS_SYNCBUSY_Msk);
    
    NVIC_ClearPendingIRQ(TC4_IRQn);
    NVIC_EnableIRQ(TC4_IRQn);
    
    while(pushKey == 'D'){
        delay_ms(100);
        keypadRoutine(0x80); //checking if still being pressed
    }
    elevatorState = moving;
    
    NVIC_DisableIRQ(TC4_IRQn);
}

void input(){    
    // so the first 4 bits of the keypad will be the rows
    // the last 4 bits is the columns
    
    // controls
    // A row 1, col 4 // this will make it go up
    // B row 2, col 4 // this will make it go down
    // C row 3, col 4 // this will close the door
    // D row 4, col 4 // this will hold the door
    // the rest is the floors
    if((elevatorINState == userFloorState) || (elevatorINState == destinationState) ){
        // if row 1, col 1 // 1
        if((pushKey >= '1') && (pushKey <= '9')){
            que[quePointer] = pushKey - '0';
            quePointer++;
            //if(elevatorINState == destinationState){
            queLen++;
            //}
            
        }
//        else{
//            elevatorINState--;
//        }
    }
//    else if(elevatorINState == directionState){
//        // if row 1, col 4 // A
//        if(pushKey == 'A'){
//            que[quePointer] = up;
//            quePointer++;
//        }
//        // if row 2, col 4 // B
//        else if(pushKey == 'B'){
//            que[quePointer] = down;
//            quePointer++;
//        }
//        else{
//            elevatorINState--;
//        }
//    }
    //elevatorINState++;
    if(elevatorINState == 3){
        elevatorINState = 0;
    }
    
    if (quePointer == 12){
        quePointer = 0;
    }
    if(pushKey == 'D'){
        holdRequested = 1; 
    }
    if(pushKey == 'C'){
        elevatorState = moving;
        idleCount = 0;
    }
    
}