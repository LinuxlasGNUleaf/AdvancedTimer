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
        TIMER_STATE state;
        TM1637Display *seg_display;
        int timer_minutes;
        const int *encoder_pins;
        const int *display_pins;
        
        bool blink_state;
        unsigned long last_blink;
        unsigned long *blink_delay;

        int old_encoder_pos;

        void checkPosition();
    public:
        RotaryEncoder *enc;

        TimerHandler(const int *enc_pins, const int *disp_pins, unsigned long *blink_delay);
        void init(void (*checkFunc)());
        void displayMinutes(int num, bool dots, bool leading_zeroes);
        void tick();
        void resetTimer();
        void setBlinkDelay(unsigned long *blink_delay);
};

#endif