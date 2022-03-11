#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <RotaryEncoder.h>

#include "SandSimulation.h"
#include "TimeHandler.h"

/*
 * ====================>> CONSTANTS <<====================
*/

//==========>> LED MATRIX <<==========
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
//====================================

//==========>> 7 SEGMENT DISPLAY <<==========
// display pins: CLK, DIO
const int disp_pins[] = {8, 7};
// rotary encoder: CLK, DT, LatchMode
const int enc_pins[] = {2, 3};
const RotaryEncoder::LatchMode mode = RotaryEncoder::LatchMode::FOUR3;
//===========================================

//==========>> SIMULATION SETTINGS <<==========
const float disp_intensity = 0.001f;

const int spawn_xrange[] = {3, 4};
const int spawn_y = 0;

const long spawn_interval = 100;
const long update_interval = 25;
//=============================================

/*
 * ====================>> OBJECTS AND FUNCTIONS <<====================
*/

TimeHandler time_handler = TimeHandler(enc_pins, disp_pins);
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

  // fill upepr half of hourglass
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
}