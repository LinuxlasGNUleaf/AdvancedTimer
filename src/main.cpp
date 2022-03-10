#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <RotaryEncoder.h>
#include "SandSimulation.h"

uint16_t constraints[] = {
    0b0000011111100000,
    0b0000001111000000,
    0b0000000110000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000110000000,
    0b0000001111000000,
    0b0000011111100000};

#define LEDMAT_TYPE MD_MAX72XX::FC16_HW
#define LEDMAT_MAX_DEVICES 2
#define LEDMAT_CLK 13                                                       // or SCK
#define LEDMAT_DATA 11                                                      // or MOSI
#define LEDMAT_CS 10                                                        // or SS
MD_MAX72XX ledmat = MD_MAX72XX(LEDMAT_TYPE, LEDMAT_CS, LEDMAT_MAX_DEVICES); // SPI hardware interface
SandSimulation sand_sim = SandSimulation(&ledmat, constraints);

#define SEG_CLK 8
#define SEG_DIO 9
TM1637Display seg_display(SEG_CLK, SEG_DIO);

#define ROT_ENC1 2
#define ROT_ENC2 3
RotaryEncoder *encoder = nullptr;

void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

// --- constants ---
const float disp_intensity = 0.001f;

const int spawn_xrange[] = {3, 4};
const int spawn_y = 0;

const long spawn_interval = 50;
const long update_interval = 25;

void fillUpperHalf();
void tickHourGlass();

void setup()
{
  Serial.begin(9600);
  Serial.println("booting...");
  randomSeed(analogRead(0));
  
  encoder = new RotaryEncoder(ROT_ENC1, ROT_ENC2, RotaryEncoder::LatchMode::FOUR3);

  Serial.print("attaching interrupts...");
  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(ROT_ENC1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_ENC2), checkPosition, CHANGE);

  Serial.print("done.\ninitializing LED matrices...");
  sand_sim.init();
  sand_sim.setIntensity(disp_intensity);
  seg_display.setBrightness(0x0f);
  
  Serial.print("done.\nboot complete. starting animation.");

  sand_sim.setUpdateIntervals(update_interval,spawn_interval);
  sand_sim.fillUpperHalf();
  sand_sim.setYRange(FIELD_SIZE, 2*FIELD_SIZE);
}

int oldPos = 0;
int newPos;
unsigned long last_update = 0;
unsigned long last_spawn = 0;
void loop()
{
  sand_sim.tickHourglass(&last_update,&last_spawn);
  newPos = encoder->getPosition();
  if (oldPos != newPos)
  {
    uint8_t oldSREG = SREG;
    cli();
    seg_display.showNumberDec(newPos, true, 4);
    oldPos = newPos;

    SREG = oldSREG;
  }
}