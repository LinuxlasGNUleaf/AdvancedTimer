#include "TimerHandler.h"

TimerHandler::TimerHandler(const int *enc_pins, bool invert_direction, const int *disp_pins, unsigned long button_threshold, unsigned long *blink_delay, uint8_t display_brightness)
{
    this->state = SELECT_TIME;
    this->timer_minutes = 0;
    this->encoder_pins = enc_pins;
    this->display_pins = disp_pins;
    this->blink_delay = blink_delay;
    this->blink_state = true;
    this->last_blink = 0;
    this->old_encoder_pos = -1;
    this->invert_direction = invert_direction;
    this->display_brightness = display_brightness;
    this->button_threshold = button_threshold;
    this->button_pressed = false;
}

void TimerHandler::init(void (*encoder_func)())
{
    enc = new RotaryEncoder(encoder_pins[0], encoder_pins[1], RotaryEncoder::LatchMode::FOUR3);
    seg_display = new TM1637Display(display_pins[0], display_pins[1]);

    pinMode(encoder_pins[2], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[0]), encoder_func, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[1]), encoder_func, CHANGE);

    seg_display->setBrightness(display_brightness);
    seg_display->clear();
}

void TimerHandler::resetTimer()
{
    if (invert_direction)
        old_encoder_pos = 1;
    else
        old_encoder_pos = -1;
    timer_minutes = 0;
    enc->setPosition(0);
}

void TimerHandler::displayMinutes(int mins, bool dots, bool leading_zeroes)
{
    if (!blink_state)
    {
        seg_display->clear();
        return;
    }
    int disp_num = ((mins / 60) * 100) + (mins % 60);

    seg_display->showNumberDecEx(disp_num, 0x80 >> 1, leading_zeroes, 4, 0);
}

void TimerHandler::tick()
{
    switch (state)
    {
        case SELECT_TIME:
            if ((blink_state && (millis() - last_blink >= blink_delay[0] || last_blink == 0)) || (!blink_state && (millis() - last_blink >= blink_delay[1] || last_blink == 0)))
            {
                last_blink = millis();
                blink_state = !blink_state;
                seg_display->setBrightness(display_brightness, blink_state);
                displayMinutes(abs(enc->getPosition()), true, true);
            }

            if (old_encoder_pos != enc->getPosition())
            {
                if ((invert_direction && enc->getPosition() > 0) || (!invert_direction && enc->getPosition() < 0))
                    enc->setPosition(0);
                old_encoder_pos = enc->getPosition();

                displayMinutes(abs(enc->getPosition()), true, true);
            }

            Serial.println(digitalRead(encoder_pins[2]));
            switch(digitalRead(encoder_pins[2])){
                case 0: //button pressed
                    button_pressed = true;
                    break;
                case 1: //button not pressed
                    if (button_pressed){
                        timer_minutes = abs(enc->getPosition());
                        button_pressed = false;
                        state = RUNNING;
                        start_time = millis();
                        last_blink = start_time;
                        displayMinutes(timer_minutes, true, true);
                    }
                    break;
            }
            break;

        case RUNNING:
            if ((blink_state && (millis() - last_blink >= blink_delay[0] || last_blink == 0)) || (!blink_state && (millis() - last_blink >= blink_delay[1] || last_blink == 0)))
            {
                last_blink = millis();
                blink_state = !blink_state;
                seg_display->setBrightness(display_brightness, blink_state);
                displayMinutes(abs(enc->getPosition()), true, true);
            }
            break;
        case PAUSED:

            break;
        case FINISHED:

            break;
        default:
            break;
    }
}

void TimerHandler::setBlinkDelay(unsigned long *blink_delay)
{
    this->blink_delay = blink_delay;
    this->last_blink = 0;
}

void TimerHandler::setDisplayBrightness(uint8_t brightness)
{
    this->display_brightness = brightness;
}