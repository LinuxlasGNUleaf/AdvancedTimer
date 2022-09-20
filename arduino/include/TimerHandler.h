#ifndef TIMERHANDLER_H
#define TIMERHANDLER_H

#include <ezBuzzer.h>
#include <RotaryEncoder.h>
#include <TM1637Display.h>
#include <config.h>

enum TIMER_STATE
{
    SELECT_MODE,
    SELECT_TIME,
    RUNNING,
    PAUSED,
    FINISHED
};

enum TIMER_MODE
{
    HH_MM_MODE,
    MM_SS_MODE
};

void rotateSegments(uint8_t* segments);

class TimerHandler
{ // handles the time display and rotary encoder
private:
    // timestamps of last action
    unsigned long timer_start_ts;
    unsigned long last_blink_ts;
    unsigned long last_display_update_ts;
    unsigned long button_pressed_ts;

    // bool flags to keep track of actions
    bool blink_state;
    bool button_previously_pressed;
    bool ignore_button_release;
    bool editing_first_value;

    // millisecond values to keep track of time
    unsigned long remaining_ms;
    unsigned long total_ms;

    // timer values configured in the SELECT TIME state and other states
    int timer_raw_value;
    unsigned int timer_stored_value1;
    unsigned int timer_stored_value2;

    // library objects
    ezBuzzer *alarm_buzzer;
    TM1637Display *seg_display;

    void encodeNumberToSegments(unsigned int num, uint8_t *segments, int start_i, int len);
    unsigned int createDisplayLiteral(unsigned long milliseconds);

    void updateDisplay();
    void clickBuzzer();
    unsigned long calculateRemainingMs();

public:
    // encoder library object
    RotaryEncoder *enc;

    // state-machine flags
    TIMER_STATE timer_state;
    TIMER_MODE timer_mode;

    TimerHandler();

    void init(void (*encoder_func)());
    void tick();
    void resetTimerHandler();

    float calculateTimerProgress();
};

#endif