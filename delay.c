#include "delay.h"

void delay_ms(int ms) {
    for (volatile int i = 0; i < ms * 333; i++);
}

void delay_us3(int us) {
    for (volatile int i = 0; i < us; i++);
}