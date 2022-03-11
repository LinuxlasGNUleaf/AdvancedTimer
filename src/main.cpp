#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <RotaryEncoder.h>

#include "SandSimulation.h"
#include "TimeHandler.h"

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
//===========================================

//==========>> SETTINGS <<==========
const float disp_intensity = 0.001f;

const unsigned long spawn_interval = 75;
const unsigned long update_interval = 25;

unsigned long blink_delay[] = {666, 333};
//=============================================

/*
 * ====================>> OBJECTS AND FUNCTIONS <<====================
*/

TimerHandler time_handler = TimerHandler(enc_pins, disp_pins, blink_delay);
SandSimulation sand_sim = SandSimulation(mat_type, spi_bus, mat_count, constraints);

void fillUpperHalf();
void tickHourGlass();

void checkPosition()
{
  time_handler.enc->tick(); // just call tick() to check the state.
}

void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(9600);

  // initialize objects & attach interrupts
  time_handler.init(checkPosition);
  sand_sim.init();

  // fill upper half of hourglass
  sand_sim.setIntensity(disp_intensity);
  sand_sim.setUpdateIntervals(update_interval, spawn_interval);
  sand_sim.fillUpperHalf();
  sand_sim.setYRange(FIELD_SIZE, 2 * FIELD_SIZE);
}

unsigned long last_update = 0;
unsigned long last_spawn = 0;
void loop()
{
  sand_sim.tickHourglass(&last_update, &last_spawn);
  time_handler.tick();
}