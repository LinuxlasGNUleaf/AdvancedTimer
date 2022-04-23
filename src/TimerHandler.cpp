#include <avr/wdt.h>
#include "TimerHandler.h"

const uint8_t SEG_STOP[4] = {
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_C | SEG_D | SEG_E | SEG_G,
    SEG_A | SEG_B | SEG_E | SEG_F | SEG_G};

const uint8_t SEG_CONT[4] = {
    SEG_A | SEG_D | SEG_E | SEG_F,
    SEG_C | SEG_D | SEG_E | SEG_G,
    SEG_C | SEG_E | SEG_G,
    SEG_D | SEG_E | SEG_F | SEG_G,
};

const uint8_t SEG_END[4] = {
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_C | SEG_E | SEG_G,
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,
    0};

void (*resetFunc)(void) = 0;

void rotateSegments(uint8_t *segments)
{
    // rotate individual segments
    for (int i = 0; i < 4; i++)
    {
        uint8_t old_segment = segments[i];
        for (int j = 0; j < 6; j++)
        {
            int new_j = (j + 3) % 6;
            if (1 & (old_segment >> j))
            { // if bit is set
                segments[i] |= 1 << new_j;
            }
            else
            {
                segments[i] &= ~(1 << new_j);
            }
        }
    }

    // flip segments horizontally
    for (int i = 0; i < 2; i++)
    {
        uint8_t temp = segments[i];
        segments[i] = segments[3 - i];
        segments[3 - i] = temp;
    }
}

void TimerHandler::encode_num(int num, uint8_t *segments)
{
    for (int i = 3; i >= 0; i--)
    {
        segments[i] = seg_display->encodeDigit(num % ((num / 10) * 10));
        num /= 10;
    }
}

TimerHandler::TimerHandler(const int *enc_pins, RotaryEncoder::LatchMode enc_latch_mode, bool enc_invert_direction,
                           const int *disp_pins, unsigned long *display_blink_ms, uint8_t display_brightness, bool display_rotated,
                           int buzzer_pin, int buzzer_frequency, int buzz_duration, bool buzzer_on_enc_action, bool buzzer_on_finish, int *buzzer_melody, int *buzzer_note_durations, int buzzer_melody_length)
{
    // encoder settings
    this->encoder_pins = enc_pins;
    this->enc_invert_direction = enc_invert_direction;

    // display settings
    this->display_pins = disp_pins;
    this->enc_latch_mode = enc_latch_mode;
    this->display_blink_ms = display_blink_ms;
    this->display_update_ms = display_update_ms;
    this->display_brightness = display_brightness;
    this->display_rotated = display_rotated;

    // buzzer settings
    this->buzzer_pin = buzzer_pin;
    this->buzzer_frequency = buzzer_frequency;
    this->buzz_duration = buzz_duration;
    this->buzzer_on_enc_action = buzzer_on_enc_action;
    this->buzzer_on_finish = buzzer_on_finish;
    this->buzzer_melody_length = buzzer_melody_length;

    this->buzzer_melody = buzzer_melody;
    this->buzzer_note_durations = buzzer_note_durations;

    // initialization
    this->state = SELECT_TIME;
    this->last_blink_ms = 0;
    this->last_enc_pos = 0;
    this->timer_minutes = 0;
    this->blink_state = false;
    this->button_previously_pressed = false;
}

void TimerHandler::init(void (*encoder_func)())
{
    enc = new RotaryEncoder(encoder_pins[0], encoder_pins[1], enc_latch_mode);
    seg_display = new TM1637Display(display_pins[0], display_pins[1]);

    pinMode(encoder_pins[2], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[0]), encoder_func, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoder_pins[1]), encoder_func, CHANGE);

    melody_buzzer = new ezBuzzer(buzzer_pin);
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
        power = blink_state || last_enc_pos != 0;
        dots = power;
        disp_num = mins_to_display(abs(last_enc_pos));
        encode_num(disp_num, segments);
        break;
    case RUNNING:
        power = true;
        dots = blink_state;
        disp_num = millis_to_display(end_time - millis());
        encode_num(disp_num, segments);
        break;
    case PAUSED:
        power = blink_state;
        dots = false;
        for (int i = 0; i < 4; i++)
            segments[i] = (last_enc_pos % 2) ? SEG_STOP[i] : SEG_CONT[i];
        break;
    case FINISHED:
        power = true;
        dots = false;
        for (int i = 0; i < 4; i++)
            segments[i] = SEG_END[i];
        break;
    default:
        break;
    }
    if (display_rotated)
        rotateSegments(segments);

    if (dots)
        segments[1] |= SEG_DP;

    seg_display->setBrightness(display_brightness, power);
    seg_display->setSegments(segments, 4, 0);
}
void TimerHandler::beepBuzzer(int duration)
{
    if (melody_buzzer->getState() == BUZZER_IDLE)
        tone(buzzer_pin, buzzer_frequency, duration);
}

void TimerHandler::tick()
{
    melody_buzzer->loop();
    unsigned long current_time = millis();

    if (state == RUNNING && current_time > end_time)
        state = FINISHED;

    if (last_enc_pos != enc->getPosition() && digitalRead(encoder_pins[2]))
    {
        if ((enc_invert_direction && enc->getPosition() > 0) || (!enc_invert_direction && enc->getPosition() < 0))
            enc->setPosition(0);
        last_enc_pos = enc->getPosition();
        if (buzzer_on_enc_action)
            beepBuzzer(buzz_duration);
        updateDisplay();
    }
    if ((blink_state && (current_time - last_blink_ms >= display_blink_ms[0])) || (!blink_state && (current_time - last_blink_ms >= display_blink_ms[1])))
    {
        blink_state = !blink_state;
        last_blink_ms = current_time;
        if (buzzer_on_finish && state == FINISHED && melody_buzzer->getState() == BUZZER_IDLE)
            melody_buzzer->playMelody(buzzer_melody, buzzer_note_durations, buzzer_melody_length);
        updateDisplay();
    }

    if (digitalRead(encoder_pins[2]))
    { // button not pressed
        if (wait_for_button_released)
        { // button release flag was set
            wait_for_button_released = false;
            button_previously_pressed = false;
        }
        else if (button_previously_pressed)
        { // button was pressed before
            int new_time = 0;
            switch (state)
            {
            case SELECT_TIME:
                /*
                 * Time selected, if time valid set state to RUNNING and calculate end time
                 */
                if (last_enc_pos == 0)
                {
                    wait_for_button_released = true;
                    break;
                }
                timer_minutes = abs(last_enc_pos);
                state = RUNNING;
                start_time = current_time;
                end_time = current_time + (timer_minutes * 60000);
                break;

            case RUNNING:
                /*
                 * Pause issued, save remaining time and set state to PAUSED
                 */
                remaining_time = end_time - current_time;
                state = PAUSED;
                break;
            case PAUSED:
                /*
                 * Pause ended, either resume or reset Arduino
                 */
                if (last_enc_pos % 2)
                { // STOP selected
                    resetFunc();
                }
                else
                { // CONTINUE selected
                    end_time = current_time + remaining_time;
                    start_time = current_time;
                    state = RUNNING;
                }
                break;
            case FINISHED:
                /*
                 * Timer stopped and button pressed, reset Arduino
                 */
                melody_buzzer->stop();
                resetFunc();
                break;
            default:
                break;
            }
            enc->setPosition(new_time);
            last_blink_ms = current_time;
            blink_state = true;
            updateDisplay();
            wait_for_button_released = true;
        }
    }
    else
    { // button pressed
        if (buzzer_on_enc_action && !button_previously_pressed)
            beepBuzzer(buzz_duration);
        button_previously_pressed = true;
    }
}

void TimerHandler::setBlinkDelay(unsigned long *display_blink_ms)
{
    this->display_blink_ms = display_blink_ms;
    this->last_blink_ms = millis();
}

void TimerHandler::setDisplayBrightness(uint8_t brightness)
{
    this->display_brightness = brightness;
}