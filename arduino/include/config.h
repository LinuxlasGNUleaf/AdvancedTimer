#ifndef CONFIG_H
#define CONFIG_H

// CONFIGURATION FILE
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <RotaryEncoder.h>
#include <EEPROM.h>
#include <hash.h>
#include <ezBuzzer.h>

// ======> PREPROCESSOR MACROS
#define DEBUG 1

#if DEBUG
#define SPRINT(msg) Serial.print(msg)
#define SPRINTLN(msg) Serial.println(msg)
#else
#define SPRINT(msg)
#define SPRINTLN(msg) 
#endif

// ======> CPP MACROS

#define INTERVAL_PASSED(start_time, interval, current_time) ((start_time + interval) < current_time)

// ======> HARDWARE / WIRING CONFIGURATION

    // ==> LED MATRIX
        
        // MATRIX SPI BUS: DIO, CLK, CS
        const int MAT_SPI_BUS[] = {11, 13, 10};

        // MODULE WIDTH
        const int MAT_WIDTH = 8;

        // MODULE COUNT
        const int MAT_MODULE_COUNT = 2;

        // MATRIX HW_TYPE
        const MD_MAX72XX::moduleType_t MAT_TYPE = MD_MAX72XX::FC16_HW;

        // MATRIX LED BRIGHTNESS
        const int MAT_BRIGHTNESS = 0;

        // MATRIX DAISY CHAIN SETUP
        /*
        *   HARDWARE LAYOUT
        *   
        * X/Y | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |
        *  ---+----+----+----+----+----+----+----+----+
        *  0  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  1  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  2  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  3  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  4  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  5  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  6  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  7  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 
        * X/Y |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
        *  ---+----+----+----+----+----+----+----+----+
        *  0  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  1  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  2  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  3  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  4  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  5  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  6  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  7  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 
        *                    /\
        *                    ||  MAT_TRANSFORM_XY()
        *                    ||
        * 
        *   SOFTWARE LAYOUT
        *   
        * X/Y |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
        *  ---+----+----+----+----+----+----+----+----+
        *  0  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  1  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  2  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  3  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  4  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  5  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  6  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  7  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 
        * X/Y |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
        *  ---+----+----+----+----+----+----+----+----+
        *  8  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        *  9  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 10  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 11  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 12  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 13  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 14  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        * 15  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
        */

        extern void MAT_TRANSFORM_XY(int *x, int *y);


        // MATRIX CONSTRAINTS
        extern uint16_t MAT_CONSTRAINTS[];
        const int MAT_FREE = 64-12; 

    // ==> 7SEG DISPLAY

        // 7SEG PINS: CLK, DIO
        const int SEG_PINS[] = {8, 9};
        
        // 7SEG LED BRIGHTNESS
        const int SEG_BRIGHTNESS = 1;

        // 7SEG DISPLAY ROTATION
        const bool SEG_ROTATED = true;


    // ==> ROTARY ENCODER

        // ENCODER PINS : CLK, DT, SW
        const int ENC_PINS[] = {2, 3, 4};
        
        // ENCODER LATCH MODE
        const RotaryEncoder::LatchMode ENC_LATCH_MODE = RotaryEncoder::LatchMode::FOUR3;
        
        // ENCODER POSITIVE DIRECTION: CW = CLOCKWISE
        const bool ENC_CW_IS_POSITIVE = true;


    // ==> BUZZER

        // BUZZER PIN
        const int BUZZER_PIN = 7;



// ======> SOFTWARE CONFIGURATION

    // ==> DEBUG
        const unsigned int SERIAL_SPEED = 9600;

    // ==> TIMER 
        // SEGMENT ARRAY TO DISPLAY "HH:MM"
        const uint8_t SEG_HH_MM[4] = {
            SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
            SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
            SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_DP,
            SEG_A | SEG_B | SEG_C | SEG_E | SEG_F
        };

        // SEGMENT ARRAY TO DISPLAY "MM:SS"
        const uint8_t SEG_MM_SS[4] = {
            SEG_A | SEG_B | SEG_C | SEG_E | SEG_F,
            SEG_A | SEG_B | SEG_C | SEG_E | SEG_F,
            SEG_A | SEG_C | SEG_D | SEG_F | SEG_G | SEG_DP,
            SEG_A | SEG_C | SEG_D | SEG_F | SEG_G
        };
        
        
        // SEGMENT ARRAY TO DISPLAY "S T O P"
        const uint8_t SEG_STOP[4] = {
            SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
            SEG_D | SEG_E | SEG_F | SEG_G,
            SEG_C | SEG_D | SEG_E | SEG_G,
            SEG_A | SEG_B | SEG_E | SEG_F | SEG_G
        };

        // SEGMENT ARRAY TO DISPLAY "C O N T"
        const uint8_t SEG_CONT[4] = {
            SEG_A | SEG_D | SEG_E | SEG_F,
            SEG_C | SEG_D | SEG_E | SEG_G,
            SEG_C | SEG_E | SEG_G,
            SEG_D | SEG_E | SEG_F | SEG_G,
        };

        // SEGMENT ARRAY TO DISPLAY "E N D  "
        const uint8_t SEG_END[4] = {
            SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
            SEG_C | SEG_E | SEG_G,
            SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,
            0
        };


    // ==> LED MATRIX
        const unsigned int MAT_GRAIN_SPAWN_MULT = 2;
        const unsigned int MAT_DISP_UPDATE_INTERVAL = 20;


    // ==> 7SEG DISPLAY

        // 7SEG BLINK DURATIONS: ON, OFF
        const unsigned int SEG_BLINK_DURATION[] = {618, 382};

        const unsigned int SEG_UPDATE_INTERVAL = 40; //25 FPS


    // ==> BUZZER

        // BUZZER CLICK FEEDBACK
        const bool BUZZER_DO_CLICK = true;

        const int BUZZER_CLICK_FREQ = 440;
        const int BUZZER_CLICK_DURATION = 5;


        // BUZZER ALARM ON FINISH
        const bool BUZZER_DO_ALARM = true;

        // BUZZER ALARM
        extern int BUZZER_ALARM[];
        extern int BUZZER_ALARM_DURATIONS[];
        extern int BUZZER_ALARM_LENGTH;

    // ==> EEPROM SETTINGS
        const int EEPROM_LENGTH = 4UL + sizeof(unsigned int) + sizeof(unsigned int);
#endif