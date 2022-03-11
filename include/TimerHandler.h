#ifndef TIME_HANDLER
#define TIME_HANDLER

#include <RotaryEncoder.h>
#include <TM1637Display.h>


enum TIMER_STATE {
    SELECT_TIME,
    PAUSED,
    RUNNING,
    FINISHED,
};

class TimerHandler
{ // handles the time display and rotary encoder
    private:
        int timer_minutes;
        unsigned start_time;

        const int *display_pins;
        uint8_t display_brightness;
        bool invert_direction;
        
        bool blink_state;
        unsigned long last_blink;
        unsigned long *blink_delay;

        unsigned long button_threshold;
        int old_encoder_pos;
        bool button_pressed;

    public:
        TM1637Display *seg_display;
        RotaryEncoder *enc;
        TIMER_STATE state;

        const int *encoder_pins;

        TimerHandler(const int *enc_pins, bool invert_direction, const int *disp_pins, unsigned long button_threshold, unsigned long *blink_delay, uint8_t display_brightness);
        void init(void (*encoder_func)());
        void displayMinutes(int num, bool dots, bool leading_zeroes);
        void tick();
        void resetTimer();
        void setBlinkDelay(unsigned long *blink_delay);
        void setDisplayBrightness(uint8_t);
};

#endif