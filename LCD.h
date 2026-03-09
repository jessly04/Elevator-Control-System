#ifndef LCD_H    /* Guard against multiple inclusion */
#define LCD_H

#include <xc.h>

void LCD_EN_PULSE();
void LCD_OUT(uint8_t HALF);
void LCD_CMD(uint8_t CMD);
void LCD_DATA(uint8_t d);
void LCD_INIT();
void LCD_CLEAR();
void LCD_WRITE_CHAR(char c);
void LCD_WRITE_STR(char s[], int size);
#endif 
