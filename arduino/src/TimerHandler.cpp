#include <avr/wdt.h>
#include "TimerHandler.h"
#include <config.h>

TimerHandler::TimerHandler()
{
    enc = NULL;
    seg_display = NULL;
    alarm_buzzer = NULL;

    resetTimerHandler();
}

void TimerHandler::init(void (*encoder_func)())
{
    // setup encoder and seg display via their libraries
    enc = new RotaryEncoder(ENC_PINS[0], ENC_PINS[1], ENC_LATCH_MODE);
    seg_display = new TM1637Display(SEG_PINS[0], SEG_PINS[1]);

    // set brightness for display
    seg_display->setBrightness(SEG_BRIGHTNESS, true);

    // setup the encoder push button
    pinMode(ENC_PINS[2], INPUT_PULLUP);

    // attach the interrupts for the encoder
    attachInterrupt(digitalPinToInterrupt(ENC_PINS[0]), encoder_func, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENC_PINS[1]), encoder_func, CHANGE);

    // setup the alarm buzzer via the library
    alarm_buzzer = new ezBuzzer(BUZZER_PIN);
}

void TimerHandler::resetTimerHandler()
{
    // timestamps of last action
    timer_start_ts = 0;
    last_blink_ts = 0;
    last_display_update_ts = 0;
    button_pressed_ts = 0;

    // bool flags to keep track of actions
    blink_state = false;
    button_previously_pressed = false;
    ignore_button_release = true;
    editing_first_value = true;

    // millisecond values to keep track of time
    remaining_ms = 0;
    total_ms = 0;

    // timer values configured in the SELECT TIME state
    timer_stored_value1 = 0;
    timer_stored_value2 = 0;
    timer_raw_value = 0;

    timer_state = SELECT_MODE;
    timer_mode = HH_MM_MODE;
}

void (*resetFunc)(void) = 0;

// rotates the display 180° by rotating each segment individually and reversing their order afterwards
void rotateSegments(uint8_t *segments)
{
    // rotate individual segments
    for (int i = 0; i < 4; i++)
    {
        uint8_t old_segment = segments[i];
        for (int j = 0; j < 6; j++)
        {
            int new_j = (j + 3) % 6;

            // if bit is set
            if (1 & (old_segment >> j))
            {
                segments[i] |= 1 << new_j;
            }
            else
            {
                segments[i] &= ~(1 << new_j);
            }
        }
    }

    // reverse segment order
    for (int i = 0; i < 2; i++)
    {
        uint8_t temp = segments[i];
        segments[i] = segments[3 - i];
        segments[3 - i] = temp;
    }
}

// writes the integer literal to the specified position in the segment array
void TimerHandler::encodeNumberToSegments(unsigned int num, uint8_t *segments, int start_i, int len)
{
    for (int i = start_i + len - 1; i >= start_i; i--)
    {
        // this seperates each digit of the integer by making use of integer casting when dividing.
        segments[i] = seg_display->encodeDigit(num % ((num / 10) * 10));
        num /= 10;
    }
}

void TimerHandler::updateDisplay()
{
    uint8_t segments[4] = {0, 0, 0, 0};

    switch (timer_state)
    {
    case SELECT_MODE:
        /*
        - displays currently selected mode
        */

        if (timer_raw_value % 2 == 0)
        {
            for (int i = 0; i < 4; i++)
                segments[i] = SEG_HH_MM[i];
        }
        else
        {
            for (int i = 0; i < 4; i++)
                segments[i] = SEG_MM_SS[i];
        }
        break;

    case SELECT_TIME:
        /*
        - currently edited value blinks, dots and other displayed value don't
        */

        if (blink_state || !editing_first_value)
            encodeNumberToSegments(timer_stored_value1, segments, 0, 2);
        if (blink_state || editing_first_value)
            encodeNumberToSegments(timer_stored_value2, segments, 2, 2);
        segments[2] |= SEG_DP;
        break;

    case RUNNING:
    {
        /*
        - digits don't blink, dots do
        - show remaining time on seg display
        */
        unsigned long remaining_ms = calculateRemainingMs();
        encodeNumberToSegments(createDisplayLiteral(remaining_ms), segments, 0, 4);
        if ((remaining_ms / 1000) % 2 == 1)
            segments[2] |= SEG_DP;
        break;
    }
    case PAUSED:
        /*
        - digits show options
        - dots are off
        */
        if (timer_raw_value % 2)
        {
            for (int i = 0; i < 4; i++)
                segments[i] = SEG_STOP[i];
        }
        else
        {
            for (int i = 0; i < 4; i++)
                segments[i] = SEG_CONT[i];
        }
        break;

    case FINISHED:
        /*
        - digits show end screen
        - dots are off
        */
        for (int i = 0; i < 4; i++)
            segments[i] = blink_state ? SEG_END[i] : 0;
        break;
    }

    if (SEG_ROTATED)
        rotateSegments(segments);

    seg_display->setSegments(segments, 4, 0);
}

void TimerHandler::clickBuzzer()
{
    // check if the alarm buzzer is not currently running
    if (alarm_buzzer->getState() == BUZZER_IDLE)
        tone(BUZZER_PIN, BUZZER_CLICK_FREQ, BUZZER_CLICK_DURATION);
}

void TimerHandler::tick()
{
    bool update_display_flag = false;

    alarm_buzzer->loop();
    unsigned long current_time = millis();
    long enc_change = enc->getPosition();

    // check if timer has finished
    if (timer_state == RUNNING && INTERVAL_PASSED(timer_start_ts, remaining_ms, current_time))
    {
        timer_state = FINISHED;

        // sound alarm if configured to do so
        if (BUZZER_DO_ALARM)
            alarm_buzzer->playMelody(BUZZER_ALARM, BUZZER_ALARM_DURATIONS, BUZZER_ALARM_LENGTH);
    }

    // if the on/off duration for the seg display blink has passed
    if (INTERVAL_PASSED(last_blink_ts, SEG_BLINK_DURATION[blink_state ? 0 : 1], current_time))
    {
        // switch blink state and set last_blink_ts accordingly
        blink_state = !blink_state;
        last_blink_ts = current_time;

        // set the update_display_flag
        update_display_flag = true;
    }

    // if encoder button is NOT being pressed
    if (digitalRead(ENC_PINS[2]))
    {
        // check if encoder was rotated
        if (enc_change != 0 && digitalRead(ENC_PINS[2]))
        {
            // invert value for a clockwise rotation as positive change
            if (ENC_CW_IS_POSITIVE)
                enc_change = -enc_change;

            if (timer_state == SELECT_TIME)
            {
                int temp_value;
                if (editing_first_value)
                    temp_value = timer_stored_value1 + enc_change;
                else
                    temp_value = timer_stored_value2 + enc_change;

                // if currently editing hour value, constrain to 0 to 10
                if (timer_mode == HH_MM_MODE && editing_first_value)
                    temp_value = constrain(temp_value, 0, 10);
                // otherwise, constrain to to 0 to 59
                else
                    temp_value = constrain(temp_value, 0, 59);

                if (editing_first_value)
                    timer_stored_value1 = temp_value;
                else
                    timer_stored_value2 = temp_value;
            }
            else
            {
                timer_raw_value += enc_change;
            }

            // reset the encoder to "neutral" position
            enc->setPosition(0);

            // click buzzer if configurated to do so
            if (BUZZER_DO_CLICK)
                clickBuzzer();

            // set the update_display_flag
            update_display_flag = true;
        }

        // ignore_button_release flag was set, unset it and ignore this button press
        if (ignore_button_release)
        {
            ignore_button_release = false;
            button_previously_pressed = false;
        }

        // button was pressed before and ignore_button_release was not set, button press is valid
        else if (button_previously_pressed)
        {
            switch (timer_state)
            {
            case SELECT_MODE:
                timer_mode = (timer_raw_value % 2 == 0) ? HH_MM_MODE : MM_SS_MODE;
                timer_state = SELECT_TIME;
                break;

            case SELECT_TIME:
                // if currently editing first value, switch to second
                if (editing_first_value)
                {
                    editing_first_value = false;
                    break;
                }

                // if first and second value are still 0, go back to mode selection
                if (timer_stored_value1 == 0 && timer_stored_value2 == 0)
                {
                    resetTimerHandler();
                    break;
                }

                if (timer_mode == HH_MM_MODE)
                    total_ms = timer_stored_value1 * 3600000UL + timer_stored_value2 * 60000UL;
                else
                    total_ms = timer_stored_value1 * 60000UL + timer_stored_value2 * 1000UL;

                remaining_ms = total_ms;
                timer_start_ts = current_time;
                timer_state = RUNNING;
                break;

            case RUNNING:
                // Pause issued, save remaining time and set state to PAUSED
                remaining_ms -= current_time - timer_start_ts;
                timer_state = PAUSED;
                break;

            case PAUSED:
                // Pause ended, either resume or reset the board
                if (timer_raw_value % 2)
                { // STOP selected
                    resetFunc();
                }
                else
                { // CONTINUE selected
                    timer_start_ts = current_time;
                    timer_state = RUNNING;
                }
                break;

            case FINISHED:
                // Timer stopped and button pressed, reset the board
                alarm_buzzer->stop();
                timer_state = SELECT_MODE;
                break;
            }

            // reset raw encoder value
            timer_raw_value = 0;

            // queue seg display update
            blink_state = true;
            update_display_flag = true;

            // wait for button release
            ignore_button_release = true;
        }
    }
    else
    { // button pressed
        if (!button_previously_pressed)
        {
            button_previously_pressed = true;
            button_pressed_ts = current_time;

            if (BUZZER_DO_CLICK)
                clickBuzzer();
        }
    }

    // update display is flag was set accordingly
    if (update_display_flag || INTERVAL_PASSED(last_display_update_ts, SEG_UPDATE_INTERVAL, current_time))
    {
        last_display_update_ts = current_time;
        updateDisplay();
    }
}

unsigned int TimerHandler::createDisplayLiteral(unsigned long milliseconds)
{
    switch (timer_mode)
    {
    case HH_MM_MODE:
    {
        unsigned int minutes = ceil(milliseconds / 60000.0f);
        return (((minutes) / 60) * 100) + ((minutes) % 60);
        break;
    }

    case MM_SS_MODE:
    {
        unsigned int seconds = ceil(milliseconds / 1000.0f);
        return ((seconds / 60) * 100) + (seconds % 60);
        break;
    }

    default:
        return 0;
    }
}

float TimerHandler::calculateTimerProgress()
{
    if (timer_state == PAUSED)
        return float(remaining_ms) / total_ms;
    if (timer_state == FINISHED)
        return 1.0f;
    return 1 - float(calculateRemainingMs()) / total_ms;
}

unsigned long TimerHandler::calculateRemainingMs()
{
    if (timer_start_ts + remaining_ms < millis())
        return 0;
    return timer_start_ts + remaining_ms - millis();
    
}