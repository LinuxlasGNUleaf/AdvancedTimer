#ifndef TIME_HANDLER
#define TIME_HANDLER

#include <RotaryEncoder.h>
#include <TM1637Display.h>

#define mins_to_display(mins) (((mins) / 60) * 100) + ((mins) % 60)
#define millis_to_display(ms) round((ms)/60000.0f)

enum TIMER_STATE {
    SELECT_TIME,
    PAUSED,
    RUNNING,
    FINISHED,
};

class TimerHandler
{ // handles the time display and rotary encoder
    private:
        unsigned long start_time;
        unsigned long end_time;
        unsigned long time_left;

        unsigned long last_blink_ms;
        long last_enc_pos;

        const int *display_pins;
        uint8_t display_brightness;
        unsigned long *blink_ms;
        unsigned long display_update_ms;

        unsigned long button_threshold;
        bool invert_direction;
        
        bool blink_state;
        bool button_pressed;

    public:
        TM1637Display *seg_display;
        RotaryEncoder *enc;
        TIMER_STATE state;

        int timer_minutes;
        const int *encoder_pins;

        TimerHandler(const int *enc_pins, bool invert_direction, unsigned long button_threshold, const int *disp_pins, unsigned long *blink_ms, uint8_t display_brightness);
        void init(void (*encoder_func)());
        void updateDisplay();
        void tick();
        void resetEncoder();
        void setBlinkDelay(unsigned long *blink_ms);
        void setDisplayBrightness(uint8_t);
};

#endif