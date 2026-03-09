#include <xc.h>
#include "keyPad.h"

volatile unsigned int colSelect = 0;
volatile unsigned int rowSelect = 0;
volatile unsigned int data = 0;
char volatile pushKey;
volatile unsigned int dataReady = 0;

static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
static const int inPutPin[4] = {0x10, 0x20, 0x40, 0x80};

void keypad_init(void){
    // sets col bits to inputs, and row bits to outputs
    PORT_REGS->GROUP[0].PORT_DIRCLR = 0b11110000; // sets the PA04-PA07 to input (col)
    // sets col high
    PORT_REGS->GROUP[0].PORT_OUTSET = 0b11110000; // drive PA04-PA07 to high (col)
    
    
    PORT_REGS->GROUP[0].PORT_DIRSET = 0b00001111; // sets the PA00-PA03 to outputs
    // sets rows low
    PORT_REGS->GROUP[0].PORT_OUTCLR = 0b00001111; // drives PA00-PA03 low
            
    // enables PMUX (this is what allows us to do hardware interupts on them)
    for(int i = 2; i < 4; i++){
        PORT_REGS->GROUP[0].PORT_PMUX[i] = 
                PORT_PMUX_PMUXE_A |
                PORT_PMUX_PMUXO_A;
    }
          
    // enable pmux, input buffer, and PULL up resistor
    // pmux routes to EIC controller, input buffer stores input
    for(int i = 4; i < 8; i++){
        PORT_REGS->GROUP[0].PORT_PINCFG[i] = PORT_PINCFG_INEN_Msk | PORT_PINCFG_PULLEN_Msk | PORT_PINCFG_PMUXEN_Msk;
    }
}
void keypadRoutine(unsigned int dataEICFlag){
    dataReady = 1;
    // set col low
    PORT_REGS->GROUP[0].PORT_OUTCLR= 0b11110000;
    
    // bit 4 int checker
    if((dataEICFlag & 0x10)){colSelect = 0;}
    
    // bit 5 int checker
    if((dataEICFlag & 0x20)){colSelect = 1;}
    
    // bit 6 int checker
    if((dataEICFlag & 0x40)){colSelect = 2;}
    
    // bit 7 int checker
    if((dataEICFlag & 0x80)){colSelect = 3;}
    
    for(int i=0; i < 4; i++){
        PORT_REGS->GROUP[0].PORT_OUTSET = (1 << i);
        data = PORT_REGS->GROUP[0].PORT_IN;
        if((data & inPutPin[colSelect])){
            rowSelect = i;
            break;
        }
        else{rowSelect = 1;}
    }
    pushKey = keymap[rowSelect][colSelect];
    // make the rows low
    PORT_REGS->GROUP[0].PORT_OUTCLR = 0b00001111;
    
    // set col HIGH
    PORT_REGS->GROUP[0].PORT_OUTSET= 0b11110000;
    
    
}

