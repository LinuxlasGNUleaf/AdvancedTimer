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
// rotary encoder: CLK, DT, LatchMode
const int enc_pins[] = {2, 3, 4};
const RotaryEncoder::LatchMode mode = RotaryEncoder::LatchMode::FOUR3;
bool invert_direction = true;
//===========================================

//==========>> SETTINGS <<==========
const int disp_intensity = 1;

const unsigned long spawn_ms = 75;
const unsigned long update_ms = 25;

unsigned long blink_ms[] = {927, 573};
uint8_t display_brightness = 3;
unsigned long button_threshold = 1500;
//=============================================

/*
 * ====================>> OBJECTS AND FUNCTIONS <<====================
*/

TimerHandler time_handler = TimerHandler(enc_pins, invert_direction, button_threshold, disp_pins, blink_ms, display_brightness);
SandSimulation sand_sim = SandSimulation(mat_type, spi_bus, mat_count, constraints);

void tickPosition()
{
  time_handler.enc->tick(); // just call tick() to check the state.
}

void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(9600);

  // initialize objects & attach interrupts
  time_handler.init(tickPosition);
  sand_sim.init();

  // fill upper half of hourglass
  sand_sim.setIntensity(disp_intensity);
  sand_sim.setUpdateIntervals(update_ms, spawn_ms);
  sand_sim.setYRange(0, FIELD_SIZE);

  unsigned long last_update = 0;
  unsigned long last_spawn = 0;
  while (!sand_sim.is_full){
    sand_sim.tickFillUpperHalf(&last_update, &last_spawn);
    time_handler.tick();
  }
}

unsigned long last_update = 0;
unsigned long last_spawn = 0;
void loop()
{
  while (time_handler.state == SELECT_TIME){
      time_handler.tick();
  }
  unsigned long hourglass_spawn_delay = sand_sim.calculateHourglassSpawnTime(time_handler.timer_minutes);
  sand_sim.setUpdateIntervals(update_ms, hourglass_spawn_delay);
  sand_sim.setYRange(FIELD_SIZE, 2 * FIELD_SIZE);

  while(time_handler.state != STOPPED){
    if (time_handler.state != PAUSED)
      sand_sim.tickHourglass(&last_update, &last_spawn);
    time_handler.tick();
  }
  while (1);
  
}