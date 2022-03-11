#include "TimeHandler.h"

TimeHandler::TimeHandler(const int *enc_pins, const int *disp_pins){
    this->state = SELECT_TIME;
    this->timer_minutes = 0;
    this->encoder_pins = enc_pins;
    this->display_pins = disp_pins;
}

void TimeHandler::init(void (*checkFunc)())
{
    enc = new RotaryEncoder(encoder_pins[0],encoder_pins[1], RotaryEncoder::LatchMode::FOUR3);
    seg_display = new TM1637Display(display_pins[0], display_pins[1]);

    attachInterrupt(digitalPinToInterrupt(encoder_pins[0]), checkFunc, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[1]), checkFunc, CHANGE);

    seg_display->setBrightness(0x0f);
}