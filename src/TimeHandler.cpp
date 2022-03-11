#include "TimeHandler.h"

TimerHandler::TimerHandler(const int *enc_pins, const int *disp_pins, unsigned long *blink_delay)
{
    this->state = SELECT_TIME;
    this->timer_minutes = 0;
    this->encoder_pins = enc_pins;
    this->display_pins = disp_pins;
    this->blink_delay = blink_delay;
    this->blink_state = true;
    this->last_blink = 0;
    this->old_encoder_pos = -1;
}

void TimerHandler::init(void (*checkFunc)())
{
    enc = new RotaryEncoder(encoder_pins[0], encoder_pins[1], RotaryEncoder::LatchMode::FOUR3);
    seg_display = new TM1637Display(display_pins[0], display_pins[1]);

    pinMode(encoder_pins[2], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[0]), checkFunc, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[1]), checkFunc, CHANGE);

    seg_display->setBrightness(0x0f);
    seg_display->clear();
}

void TimerHandler::resetTimer()
{
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
            displayMinutes(enc->getPosition(), true, true);
        }

        if (old_encoder_pos != enc->getPosition())
        {
            if (enc->getPosition() < 0)
            {
                enc->setPosition(0);
            }
            old_encoder_pos = enc->getPosition();
            displayMinutes(enc->getPosition(), true, true);
        }
        break;
    case PAUSED:

        break;
    case RUNNING:

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