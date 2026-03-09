#ifndef LCD_PINS_H    /* Guard against multiple inclusion */
#define LCD_PINS_H

enum LCD_Pins {
    RS = (1 << 16),
    EN = (1 << 17),
    D4 = (1 << 22),
    D5 = (1 << 23),
    D6 = (1 << 24),
    D7 = (1 << 25),
    DMask = D4 | D5 | D6 | D7
};

enum LCD_CMD {
    Clear = 0x01, //0001
    Home = 0x02, //0010
    LCDOn = 0x0C, //1100
    EMS = 0x06, //0111
    DDRM = 0x80 //10000000
};

#endif 
