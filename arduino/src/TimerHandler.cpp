#include <avr/wdt.h>
#include "TimerHandler.h"
#include <config.h>

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

TimerHandler::TimerHandler()
{
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
    enc = new RotaryEncoder(ENC_PINS[0], ENC_PINS[1], ENC_LATCH_MODE);
    seg_display = new TM1637Display(SEG_PINS[0], SEG_PINS[1]);

    pinMode(ENC_PINS[2], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENC_PINS[0]), encoder_func, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENC_PINS[1]), encoder_func, CHANGE);

    melody_buzzer = new ezBuzzer(BUZZER_PIN);
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
    if (SEG_ROTATED)
        rotateSegments(segments);

    if (dots)
        segments[1] |= SEG_DP;

    seg_display->setBrightness(SEG_BRIGHTNESS, power);
    seg_display->setSegments(segments, 4, 0);
}
void TimerHandler::clickBuzzer()
{
    if (melody_buzzer->getState() == BUZZER_IDLE)
        tone(BUZZER_PIN, BUZZER_CLICK_FREQ, BUZZER_CLICK_DURATION);
}

void TimerHandler::tick()
{
    melody_buzzer->loop();
    unsigned long current_time = millis();

    if (state == RUNNING && current_time > end_time)
        state = FINISHED;

    if (last_enc_pos != enc->getPosition() && digitalRead(ENC_PINS[2]))
    {
        if ((ENC_INVERT_DIRECTION && enc->getPosition() > 0) || (!ENC_INVERT_DIRECTION && enc->getPosition() < 0))
            enc->setPosition(0);
        last_enc_pos = enc->getPosition();
        if (BUZZER_DO_CLICK)
            clickBuzzer();
        updateDisplay();
    }
    if ((blink_state && (current_time - last_blink_ms >= SEG_BLINK_DURATIONS[0])) || (!blink_state && (current_time - last_blink_ms >= SEG_BLINK_DURATIONS[1])))
    {
        blink_state = !blink_state;
        last_blink_ms = current_time;
        if (BUZZER_DO_ALARM && state == FINISHED && melody_buzzer->getState() == BUZZER_IDLE)
            melody_buzzer->playMelody(BUZZER_ALARM, BUZZER_ALARM_DURATIONS, BUZZER_ALARM_LENGTH);
        updateDisplay();
    }

    if (digitalRead(ENC_PINS[2]))
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
        if (BUZZER_DO_CLICK && !button_previously_pressed)
            clickBuzzer();
        button_previously_pressed = true;
    }
}