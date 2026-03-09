#include <xc.h>
#include "delay.h"
#include "LCD.h"
#include "LCD_DEF.h"

// Since the LCD works on 5V and the SAM only works on 3.3V I had to step up the output
// I only had a ULN2003 on hand which inverts the signals. So all my signals are inverted
// the LCD you have may work on 3.3V the one i stole was broken and so i used my own
// but it uses the same driver as new haven so this should work
// just flip the macros below CLR -> SET and vise versa
#define LCD_HIGH(pin)  PORT_REGS->GROUP[0].PORT_OUTCLR = (pin)
#define LCD_LOW(pin)   PORT_REGS->GROUP[0].PORT_OUTSET = (pin)

// Enable pulse for the LCD display
void LCD_EN_PULSE(){
    delay_us3(1);
    LCD_HIGH(EN);
    // needs to be at least 450ns // best i can do is 3000ns
    delay_us3(10);
    LCD_LOW(EN);
}

// I use 8 bit integer here because the commands are only 2 hex
void LCD_OUT(uint8_t HALF){
    // clear all the Data out 
    LCD_LOW(DMask);
    // set them based on the command
    if(HALF & 0x1){LCD_HIGH(D4);};
    if(HALF & 0x2){LCD_HIGH(D5);};
    if(HALF & 0x4){LCD_HIGH(D6);};
    if(HALF & 0x8){LCD_HIGH(D7);};
    
    delay_us3(1);
    // Write to LCD register
    LCD_EN_PULSE();
}
// As for the order of data transfer, the four high order bits are transferred before the four low order bits
void LCD_CMD(uint8_t CMD){
    // RS = 0 means CMD register, RS=1 means data register
    LCD_LOW(RS);
    LCD_OUT((CMD >> 4) & 0x0F); // last 4 bits
    LCD_OUT(CMD & 0x0F); // first 4 bits
    
   // clear display and return home take a stupid long time
   // so delay when they come on
    if((CMD == Clear) || (CMD == Home)){delay_ms(5);} //im putting 5 because the delay function i used an estimate lol
    else {delay_us3(200);} // the rest should be fine
    
}

void LCD_DATA(uint8_t d){
    LCD_HIGH(RS); // when RS is 1, its in data mode
    
    LCD_OUT((d >> 4) & 0x0F); // last 4 bits
    LCD_OUT(d & 0x0F); // first 4 bits
    // delay a bit
    delay_ms(1);
            
}

void LCD_INIT(){
    // configure the LCD control pins as output
    PORT_REGS->GROUP[0].PORT_DIRSET = RS | EN | DMask;
    // make sure all outputs are 0
    LCD_LOW(RS | EN | DMask);
    
    // give the LCD time to power on  more than 15ms minimum
    delay_ms(20);
    // these commands come straight from the datasheet
    // function set DL = LOW (4 bit mode), N = High (2 line), F = low (5x8 pixel format)
    
    // the wakey wakey protocal
    LCD_OUT(0x3);
    delay_ms(5);
    LCD_OUT(0x3);
    delay_ms(1);
    LCD_OUT(0x03);
    delay_ms(1);
    
    // sets it in 4 bit mode
    LCD_OUT(0x02); // hex mode to set it in 4 bit mode
    delay_ms(1);
    
    // function set
    // N = 1, F = 0; 10xx
    // repeat twice
    LCD_CMD(0x28);
    LCD_CMD(0x28);
    
    // clear display
    LCD_CMD(Clear);
    
    // Entry Mode Set
    LCD_CMD(EMS);  
    
    // display ON
    LCD_CMD(LCDOn);
    delay_ms(1);
    
}

// clears display
void LCD_CLEAR(){
    LCD_CMD(Clear);
}

void LCD_WRITE_CHAR(char c){
    // have to convert the char to uint8_t
    // LCD characters are in ASC2 so you can use char data type
    LCD_DATA((uint8_t)c);
}

// helper function to write strings
void LCD_WRITE_STR(char s[], int size){
    for(int i = 0; i < size; i++){
        LCD_DATA((uint8_t)s[i]);
    }
}