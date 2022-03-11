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

class TimeHandler
{ // handles the time display and rotary encoder
    private:
        TIMER_STATE state;
        TM1637Display *seg_display;
        int timer_minutes;
        const int *encoder_pins;
        const int *display_pins;
        void checkPosition();

    public:
        RotaryEncoder *enc;

        TimeHandler(const int *enc_pins, const int *disp_pins);
        void init(void (*checkFunc)());
};

#endif