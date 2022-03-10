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
  randomSeed(analogRead(0));

  encoder = new RotaryEncoder(ROT_ENC1, ROT_ENC2, RotaryEncoder::LatchMode::FOUR3);

  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(ROT_ENC1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_ENC2), checkPosition, CHANGE);

  Serial.begin(9600);
  Serial.println("booted successfully.");
  sand_sim.init();
  sand_sim.setIntensity(disp_intensity);
  seg_display.setBrightness(0x0f);

  fillUpperHalf();
  delay(1000);
  sand_sim.setYRange(FIELD_SIZE, 2*FIELD_SIZE);
}

unsigned long lastSpawn = 0;
unsigned long lastUpdate = 0;
int oldPos = 0;
int newPos;

void loop()
{
  tickHourGlass();
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

void tickHourGlass()
{
  if (millis() - lastUpdate >= update_interval || lastUpdate == 0)
  {
    uint8_t oldSREG = SREG;
    cli();

    lastUpdate = millis();
    sand_sim.updateField();

    SREG = oldSREG;
  }
  if (millis() - lastSpawn >= spawn_interval || lastSpawn == 0)
  {
    uint8_t oldSREG = SREG;
    cli();

    lastSpawn = millis();
    sand_sim.spawnGrainInRegion(spawn_xrange);
    sand_sim.removeGrainFromRegion(0,FIELD_SIZE-1);

    if (sand_sim.is_full)
    {
      while(1);
    }

    SREG = oldSREG;
  }
}

void fillUpperHalf()
{
  sand_sim.setYRange(0,FIELD_SIZE);
  int xrange[] = {0,7};
  unsigned long lastSpawn = 0;
  unsigned long lastUpdate = 0;
  while (!sand_sim.is_full)
  {
    if (millis() - lastUpdate >= update_interval || lastUpdate == 0)
    {
      uint8_t oldSREG = SREG;
      cli();

      lastUpdate = millis();
      sand_sim.updateField();

      SREG = oldSREG;
    }
    if (millis() - lastSpawn >= spawn_interval || lastSpawn == 0)
    {
      uint8_t oldSREG = SREG;
      cli();

      lastSpawn = millis();
      sand_sim.spawnGrainInRegion(xrange);
      SREG = oldSREG;
    }
  }
}