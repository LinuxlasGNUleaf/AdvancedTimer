#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <RotaryEncoder.h>

#include "SandSimulation.h"
#include "TimerHandler.h"

/*
 * ====================>> CONSTANTS <<====================
 */

//==========>> SAND SIMULATION <<==========
// spi_bus: DIO, CLK, CS
const int spi_bus[] = {11, 13, 10};
// matrix count
const int mat_count = 2;
// matrix HW type
const MD_MAX72XX::moduleType_t mat_type = MD_MAX72XX::FC16_HW;
// matrix field constraints
uint16_t constraints[] = {
    0b0000011111100000,
    0b0000001111000000,
    0b0000000110000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000110000000,
    0b0000001111000000,
    0b0000011111100000};
//=========================================

//==========>> TIMER SETTINGS <<==========
// display pins: CLK, DIO
const int disp_pins[] = {8, 9};
bool display_rotated = true;

// rotary encoder: CLK, DT, SW
const int enc_pins[] = {2, 3, 4};
const RotaryEncoder::LatchMode enc_latch_mode = RotaryEncoder::LatchMode::FOUR3;
bool enc_invert_direction = true;
//===========================================

//==========>> BUZZER SETTINGS <<==========
const int buzzer_pin = 7;

const int buzzer_frequency = 440;
const int buzz_duration = 5;

const bool buzzer_on_enc_action = true;
const bool buzzer_on_finish = true;

int buzzer_melody[] = {
    NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4,
    NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4,
    NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
    NOTE_A4};

int buzzer_note_durations[] = {
    8, 8, 4, 4,
    8, 8, 4, 4,
    8, 8, 4, 4,
    2};

int buzzer_melody_length = sizeof(buzzer_melody) / sizeof(buzzer_melody[0]);

//=========================================

//==========>> SETTINGS <<==========
const int disp_intensity = 0;

const unsigned long spawn_ms = 75;
const unsigned long update_ms = 25;

unsigned long display_blink_ms[] = {927, 573};
uint8_t display_brightness = 1;
//=============================================

/*
 * ====================>> OBJECTS AND FUNCTIONS <<====================
 */

TimerHandler time_handler = TimerHandler(enc_pins, enc_latch_mode, enc_invert_direction,
                                         disp_pins, display_blink_ms, display_brightness, display_rotated,
                                         buzzer_pin, buzzer_frequency, buzz_duration, buzzer_on_enc_action, buzzer_on_finish, buzzer_melody, buzzer_note_durations, buzzer_melody_length);
SandSimulation sand_sim = SandSimulation(mat_type, spi_bus, mat_count, constraints);

void tickPosition()
{
  time_handler.enc->tick(); // just call tick() to check the state.
}

void setup()
{
  randomSeed(analogRead(0));

  // initialize objects & attach interrupts
  time_handler.init(tickPosition);
  sand_sim.init();

  // fill upper half of hourglass
  sand_sim.setIntensity(disp_intensity);
  sand_sim.setUpdateIntervals(update_ms, spawn_ms);
  sand_sim.setYRange(0, FIELD_SIZE);

  unsigned long last_update = 0;
  unsigned long last_spawn = 0;
  while (!sand_sim.is_full)
  {
    sand_sim.tickFillUpperHalf(&last_update, &last_spawn);
    time_handler.tick();
  }
}

unsigned long last_update = 0;
unsigned long last_spawn = 0;
void loop()
{
  while (time_handler.state == SELECT_TIME)
  {
    time_handler.tick();
  }
  unsigned long hourglass_spawn_delay = sand_sim.calculateHourglassSpawnTime(time_handler.timer_minutes);
  sand_sim.setUpdateIntervals(update_ms, hourglass_spawn_delay);
  sand_sim.setYRange(FIELD_SIZE, 2 * FIELD_SIZE);

  while (true)
  {
    sand_sim.tickHourglass(&last_update, &last_spawn, time_handler.state);
    time_handler.tick();
  }
}