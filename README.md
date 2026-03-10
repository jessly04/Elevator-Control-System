# Elevator-Control-System
A real-time elevator control system implemented on ATSAMD21E18A microcontroller. This project simulates a 9-story building using a state-machine approach, integrating hardware interrupts, timers, and custom peripheral drivers
### Key Technical Features
- **Register-Level Programming:** Written entirely in C
- **Programmer:** MPLab Snap In-Circuit Debugger/Programmer
- **State Machine Architecture:** Manages multiple states including *Idle*, *Moving*, *Door Hold*, and *Emergency Force Close*.
- **Interrupt-Driven Input:** Utilizes External Interrupt Controller (EIC) for a 4x4 matrix keypad with a custom software debounce routine.
- **Hardware Timers (TC4/TC5):**
  - **TC5:** Manages the 2-second floor to floor travel intervals
  - **TC4:** Monitors safety protocols (10-second door-hold limit)
- **Custom LCD Driver:** Developed a 4-bit interface for a 16x2 LCD, including a signal inversion layer for ULN2003
### Hardware
- **Microcontroller:** ATSAMD21E18A
- **Input:** 4x4 Matrix Keypad
- **Output:** 16x2 LCD
- **Signal Conditioning:** ULN2003 (Inverter/Driver)
- **Indicators:** LEDs (Green, Red, Yellow, and White)
### Challenges & Solutions
#### 1. Signal Inversion (ULN2003)
Due to the ATSAMD21 operating at 3.3V and the LCD requiring 5V, we used a ULN2003 to step up the voltage. Since the ULN2003 inverts logic, we implemented macro-level signal flipping in LCD.c to ensure software logic remained intuitive while hardware signals were correctly translated
#### 2. Precise Timing & Debouncing
Handling button presses on the keypad often results in "bouncing" that causes mechanical switch noise. So, we implemented a timer-based debounce using TC5 to capture the count value during an interrupt and ignore follow-up triggers for a quarter of a second ensuring clean input without blocking the main execution loop.
### Project Structure
- main.c: Core system initilization and main loop
- elevator.c: Logic for the elevator state machine and queue management as well as implements core asynchronous queue and state logic
- LCD.c: Register-level driver for the 16x2 display
- keyPad.c: Matrix scanning and EIC interrupt handling
- timerSetUp.c: Configurations for hardware timers (TC4/TC5) and NVIC (Nested Vectored Interrupt Controller) interrupt as well as configures the internal GCLK (Generic Clock)
#### Project Collaboration
This project was co-developed by Jessica Ly and Daniel Meija Mendez. We worked together across the full stack, including hardware interfacing, register-level C programming, and system debugging.
