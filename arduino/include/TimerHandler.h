#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include <RotaryEncoder.h>
#include <TM1637Display.h>
#include <ezBuzzer.h>
#include <config.h>

#define mins_to_display(mins) (((mins) / 60) * 100) + ((mins) % 60)
#define millis_to_display(ms) mins_to_display(round((ms) / 60000.0f))

enum TIMER_STATE
{
    SELECT_TIME,
    RUNNING,
    PAUSED,
    FINISHED
};

void rotateSegments(uint8_t* segments);

class TimerHandler
{ // handles the time display and rotary encoder
private:
    unsigned long start_time;
    unsigned long end_time;
    unsigned long remaining_time;

    unsigned long last_blink_ms;
    long last_enc_pos;

    ezBuzzer *melody_buzzer;

    unsigned long button_threshold;
    bool enc_invert_direction;

    bool blink_state;
    bool button_previously_pressed;
    bool wait_for_button_released;

    TM1637Display *seg_display;

    void encode_num(int num, uint8_t *segments);


public:
    RotaryEncoder *enc;
    TIMER_STATE state;

    int timer_minutes;

    TimerHandler();
    void init(void (*encoder_func)());
    void updateDisplay();
    void tick();
    void clickBuzzer();
};

#endif