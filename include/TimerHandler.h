#ifndef TIME_HANDLER
#define TIME_HANDLER

#include <RotaryEncoder.h>
#include <TM1637Display.h>
#include "ezBuzzer.h"

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

    RotaryEncoder::LatchMode latch_mode;

    const int *display_pins;
    uint8_t display_brightness;
    unsigned long *blink_ms;
    unsigned long display_update_ms;
    bool is_rotated;

    int buzzer_pin;
    int frequency;
    int buzz_duration;
    bool buzz_on_turn;
    bool buzz_on_finish;
    int *melody;
    int *note_durations;

    ezBuzzer *melody_buzzer;

    unsigned long button_threshold;
    bool invert_direction;

    bool blink_state;
    bool button_previously_pressed;
    bool wait_for_button_released;

    TM1637Display *seg_display;

    void encode_num(int num, uint8_t *segments);


public:
    RotaryEncoder *enc;
    TIMER_STATE state;

    int timer_minutes;
    const int *encoder_pins;

    TimerHandler(const int *enc_pins, RotaryEncoder::LatchMode latch_mode, bool invert_direction, unsigned long button_threshold,
                 const int *disp_pins, unsigned long *blink_ms, uint8_t display_brightness, bool is_rotated,
                 const int buzzer_pin, const int frequency, const int buzz_duration, const bool buzz_on_turn, const bool buzz_on_finish, int *melody, int *note_durations);
    void init(void (*encoder_func)());
    void updateDisplay();
    void tick();
    void setBlinkDelay(unsigned long *blink_ms);
    void setDisplayBrightness(uint8_t);
    void beepBuzzer(int duration);
};

#endif