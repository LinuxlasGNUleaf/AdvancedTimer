#include <ezBuzzer.h>
#include <config.h>

// ======> LED MATRIX

uint16_t MAT_CONSTRAINTS[] = {
    0b0000011111100000,
    0b0000001111000000,
    0b0000000110000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000110000000,
    0b0000001111000000,
    0b0000011111100000};

void MAT_TRANSFORM_XY(int *x, int *y)
{
    if (*y < 8)
    {
        *x = 15 - *x;
    }
    else
    {
        *x = 7 - *x;
        *y -= 8;
    }
}


// ======> BUZZER

int BUZZER_ALARM[] = {
    NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4,
    NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4,
    NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
    NOTE_A4};

int BUZZER_ALARM_DURATIONS[] = {
    8, 8, 4, 4,
    8, 8, 4, 4,
    8, 8, 4, 4, 2};

int BUZZER_ALARM_LENGTH = sizeof(BUZZER_ALARM) / sizeof(BUZZER_ALARM[0]);
