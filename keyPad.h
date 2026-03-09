
#ifndef KEYPAD_H    /* Guard against multiple inclusion */
#define KEYPAD_H

#include <xc.h>

extern volatile char pushKey;
extern volatile unsigned int colSelect;
extern volatile unsigned int rowSelect;
extern volatile unsigned int data;
extern volatile unsigned int dataReady;


void keypad_init(void);

void keypadRoutine(unsigned int dataEICFlag);

#endif 


