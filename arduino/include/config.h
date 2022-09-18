#ifndef CONFIG_H
#define CONFIG_H

// CONFIGURATION FILE
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <RotaryEncoder.h>
#include <ezBuzzer.h>


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
        
        // ENC INVERT DIRECTION
        const bool ENC_INVERT_DIRECTION = true;


    // ==> BUZZER

        // BUZZER PIN
        const int BUZZER_PIN = 7;



// ======> SOFTWARE CONFIGURATION

    // ==> LED MATRIX BEHAVIOR
        const unsigned long MAT_GRAIN_SPAWN_INTERVAL = 75;
        const unsigned long MAT_DISP_UPDATE_INTERVAL = 25;

    // ==> 7SEG DISPLAY BEHAVIOR

        // 7SEG BLINK DURATIONS: ON, OFF
        const unsigned long SEG_BLINK_DURATIONS[] = {927, 573};


    // ==> BUZZER ACTION

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
#endif