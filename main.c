#include <xc.h>
#include "delay.h"
#include "timerSetUp.h"
#include "keyPad.h"
#include "elevator.h"
#include "LCD.h"

volatile unsigned int led_state = 0;
volatile unsigned int busy = 0;

#define GREEN_LED_PIN (1 << 8)
#define RED_LED_PIN (1 << 9)
#define YELLOW_LED_PIN (1 << 10)




volatile unsigned int dataEICFlag;
volatile unsigned int TC5CountVal;

void TC4_Handler(void){
    elevatorState = forceClose;
    TC4_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0_Msk;
    NVIC_DisableIRQ(TC4_IRQn);
    
}


void TC5_Handler(void)
{
    if(TC5_REGS->COUNT16.TC_INTFLAG & TC_INTFLAG_MC1_Msk){ // quarter second timer goes off
        dataReady=0; // we will now be able to take another key
        TC5_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_MC1_Msk;
        TC5_REGS->COUNT16.TC_INTENCLR = TC_INTENCLR_MC1_Msk; // disables the quarter second interupt
    }
    else{ // every 2 seconds the TC5 handler goes off
        if(elevatorState != holdDoor){ // if the door isnt being held proceed as normal
            elevator();
        }
        // remmeber to reset the INT flag or else the handler will run again immeditly
        TC5_REGS->COUNT16.TC_INTFLAG = TC_INTFLAG_MC0_Msk; //Mc0 is the int for 2 seconds // mc1 is the quarter second interupt
    }
}

void Debounce_time(unsigned int count){
    count = count + 3906; // prescaler for timer is 64, so a quarter of a second with a 1 million cycle clock rate is 3906 ticks
    
    // since cc0 is 2 seconds if cc1 is greater than cc0, then cc0 will never go off
    if(count > 31250){
        count = 3906;
    }
    TC5_REGS->COUNT16.TC_CC[1] = count;
    while (TC5_REGS->COUNT16.TC_STATUS & TC_STATUS_SYNCBUSY_Msk);
    
    TC5_REGS->COUNT16.TC_INTENSET = TC_INTENSET_MC1_Msk;
    
    
    
}

void EIC_Handler(void){
    dataEICFlag = EIC_REGS->EIC_INTFLAG;
    EIC_REGS->EIC_CTRL = 0b00; // stop EIC prevents another iturupt from re occuring
    while(EIC_REGS->EIC_STATUS & EIC_STATUS_SYNCBUSY_Msk);
    //LCD_CLEAR();
    TC5CountVal = (TC5_REGS->COUNT16.TC_COUNT & TC_COUNT16_COUNT_COUNT_Msk);
    
    if(!dataReady){
        Debounce_time(TC5CountVal);
        keypadRoutine(dataEICFlag);
        input();
    }
    
    //LCD_WRITE_CHAR(pushKey);
    
    EIC_REGS->EIC_CTRL = 0b10;
    while(EIC_REGS->EIC_STATUS & EIC_STATUS_SYNCBUSY_Msk);
    //PORT_REGS ->GROUP[0].PORT_OUTTGL = RED_LED_PIN;
    
    // writing a 1 to this register resets the interupt flag
    EIC_REGS->EIC_INTFLAG = 0b11111111;
    

}



static void External_Pin_INT(){
    PM_REGS->PM_APBAMASK |= PM_APBAMASK_EIC_Msk;
    
    GCLK_REGS->GCLK_CLKCTRL = 
        GCLK_CLKCTRL_ID_EIC 
        | GCLK_CLKCTRL_GEN_GCLK0 
        | GCLK_CLKCTRL_CLKEN_Msk;
    while(GCLK_REGS->GCLK_STATUS & EIC_STATUS_SYNCBUSY_Msk);
    
    
    EIC_REGS->EIC_EVCTRL = 
           EIC_EVCTRL_EXTINTEO4_Msk |
           EIC_EVCTRL_EXTINTEO5_Msk |
           EIC_EVCTRL_EXTINTEO6_Msk |
           EIC_EVCTRL_EXTINTEO7_Msk;
    while(EIC_REGS->EIC_STATUS & EIC_STATUS_SYNCBUSY_Msk);
    // enables input sense and filter on pins 0-8
    // will enable interupt on rising edge
    EIC_REGS->EIC_CONFIG[0] = 
            EIC_CONFIG_SENSE4_FALL |
            EIC_CONFIG_FILTEN4_Msk |
            
            EIC_CONFIG_SENSE5_FALL |
            EIC_CONFIG_FILTEN5_Msk |
            
            EIC_CONFIG_SENSE6_FALL |
            EIC_CONFIG_FILTEN6_Msk |
            
            EIC_CONFIG_SENSE7_FALL |
            EIC_CONFIG_FILTEN7_Msk;
    while(EIC_REGS->EIC_STATUS & EIC_STATUS_SYNCBUSY_Msk);
    
    EIC_REGS->EIC_INTENSET = 0b11110000; // enables external interupt on PA04-PA07
    
    // Now the microcontroller should be listening for an INT from the IO pins
    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_EnableIRQ(EIC_IRQn);
    __enable_irq();
    
    // enable EIC
    EIC_REGS->EIC_CTRL = EIC_CTRL_ENABLE_Msk;
    while(EIC_REGS->EIC_STATUS &  EIC_STATUS_SYNCBUSY_Msk);
}

//void LCD_Handler(void){
//    
//}

int main(void)
{
    
    PM_REGS->PM_APBBMASK |= PM_APBBMASK_PORT_Msk;
    
    LCD_INIT();
    LCD_CMD(0x80);
    LCD_CLEAR();
//    LCD_WRITE_STR("Hello", 5);
//    LCD_WRITE_STR(" world", 6);
    
    // Configure PA17 as output for the LED
    PORT_REGS->GROUP[0].PORT_DIRSET = GREEN_LED_PIN;
    PORT_REGS->GROUP[0].PORT_DIRSET = RED_LED_PIN;
    PORT_REGS->GROUP[0].PORT_DIRSET = YELLOW_LED_PIN;
    PORT_REGS->GROUP[0].PORT_DIRSET = BUZZER;
    
    Timer4_Init();
    Timer5_Init();

    keypad_init();
    delay_ms(1);
    External_Pin_INT();
    
    
            
    // inittialize the display
    //void LCD_CLEAR(); 
   
    // Initialize our timer + interrupt

    
    while (1){
        // you need some delay since the clock moves faster than the registers can update
        // but also not really because 1Mhz is not that fast but he might still grill you if you dont
        if(holdRequested){
            holdRequested = 0;
            hold();    
        }
    }
    return 0; 
}