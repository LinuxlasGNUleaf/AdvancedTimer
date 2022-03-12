#include "TimerHandler.h"

const uint8_t SEG_STOP[] = {
    SEG_A | SEG_B | SEG_D | SEG_G | SEG_E,
    SEG_B | SEG_C | SEG_D,
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
	};

const uint8_t SEG_CONT[] = {
    SEG_A | SEG_D | SEG_E | SEG_F,
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    SEG_B | SEG_C | SEG_D,
	};

void TimerHandler::encode_num(int num, uint8_t *segments){
    for (int i = 3; i >= 0; i--)
    {
        segments[i] = seg_display->encodeDigit(num % ((num/10)*10));
        num /= 10;
    }
}

TimerHandler::TimerHandler(const int *enc_pins, bool invert_direction, unsigned long button_threshold, const int *disp_pins, unsigned long *blink_ms, uint8_t display_brightness)
{
    this->encoder_pins = enc_pins;
    this->invert_direction = invert_direction;
    this->button_threshold = button_threshold;

    this->display_pins = disp_pins;
    this->blink_ms = blink_ms;
    this->display_update_ms = display_update_ms;
    this->display_brightness = display_brightness;

    this->state = SELECT_TIME;
    this->last_blink_ms = 0;
    this->last_enc_pos = 0;
    this->timer_minutes = 0;
    
    this->blink_state = true;
    this->button_pressed = false;
}

void TimerHandler::init(void (*encoder_func)())
{
    enc = new RotaryEncoder(encoder_pins[0], encoder_pins[1], RotaryEncoder::LatchMode::FOUR3);
    seg_display = new TM1637Display(display_pins[0], display_pins[1]);

    pinMode(encoder_pins[2], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[0]), encoder_func, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[1]), encoder_func, CHANGE);
}

void TimerHandler::resetEncoder()
{
    if (invert_direction)
        last_enc_pos = 1;
    else
        last_enc_pos = -1;
    timer_minutes = 0;
    enc->setPosition(0);
}

void TimerHandler::updateDisplay()
{
    bool power;
    bool dots;
    int disp_num;

    uint8_t segments[4];

    switch (state)
    {
    case SELECT_TIME:
        power = blink_state;
        dots = blink_state;
        disp_num = mins_to_display(abs(enc->getPosition()));
        encode_num(disp_num, segments);
        break;
    case RUNNING:
        power = true;
        dots = blink_state;
        disp_num = millis_to_display(end_time-millis());
        encode_num(disp_num, segments);
        break;
    case PAUSED:
        power = true;
        dots = false;
        for (int i = 0; i < 4; i++)
            segments[i] = (enc->getPosition() % 2)?SEG_CONT[i]:SEG_STOP[i];
    default:
        break;
    }

    if (dots)
        segments[1] |= SEG_DP;

    seg_display->setBrightness(display_brightness, power);
    seg_display->setSegments(segments,4,0);
}

void TimerHandler::tick()
{
    unsigned long current_time = millis();

    if (last_enc_pos != enc->getPosition()) 
    {
        if ((invert_direction && enc->getPosition() > 0) || (!invert_direction && enc->getPosition() < 0))
            enc->setPosition(0);
        last_enc_pos = enc->getPosition();
        updateDisplay();
    }
    if ((blink_state && (current_time - last_blink_ms >= blink_ms[0])) || (!blink_state && (current_time - last_blink_ms >= blink_ms[1])))
    {
        blink_state = !blink_state;
        last_blink_ms = current_time;
        updateDisplay();
    }


    switch (state)
    {
        case SELECT_TIME:
            if(digitalRead(encoder_pins[2]))
            { //button not pressed
                if (button_pressed)
                {
                    timer_minutes = abs(enc->getPosition());
                    button_pressed = false;
                    if (timer_minutes == 0)
                        return;
                    enc->setPosition(0);
                    state = RUNNING;
                    start_time = current_time;
                    end_time = current_time + timer_minutes*60000;
                    last_blink_ms = start_time;
                    blink_state = true;
                    updateDisplay();
                }
            }
            else
            { //button pressed
                button_pressed = true;
            }
            break;

        case RUNNING:
            if(digitalRead(encoder_pins[2]))
            { //button not pressed
                if (button_pressed)
                {
                    time_left = end_time - current_time;
                    button_pressed = false;
                    state = PAUSED;
                    resetEncoder();
                    seg_display->setSegments(SEG_CONT,4,0);
                }
            }
            else
            { //button pressed
                button_pressed = true;
            }
            break;

        case PAUSED:
            if (last_enc_pos != enc->getPosition())
            {
                if (enc->getPosition() % 2 == 0)
                    seg_display->setSegments(SEG_CONT,4,0);
                else
                    seg_display->setSegments(SEG_STOP,4,0);
            }
            switch(digitalRead(encoder_pins[2])){
                case 0: //button pressed
                    button_pressed = true;
                    break;
                case 1: //button not pressed
                    if (button_pressed){
                        time_left = end_time - current_time;
                        button_pressed = false;
                        state = PAUSED;
                        resetEncoder();
                        seg_display->setSegments(SEG_CONT,4,0);
                    }
                    break;
            }
            break;
        case FINISHED:

            break;
        default:
            break;
    }
}

void TimerHandler::setBlinkDelay(unsigned long *blink_ms)
{
    this->blink_ms = blink_ms;
    this->last_blink_ms = millis();
}

void TimerHandler::setDisplayBrightness(uint8_t brightness)
{
    this->display_brightness = brightness;
}