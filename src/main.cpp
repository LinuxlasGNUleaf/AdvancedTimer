#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include "SandSimulation.h"

#define LEDMAT_TYPE MD_MAX72XX::FC16_HW
#define LEDMAT_MAX_DEVICES 1
#define LEDMAT_CLK 13  // or SCK
#define LEDMAT_DATA 11 // or MOSI
#define LEDMAT_CS1 9   // or SS
#define LEDMAT_CS2 10   // or SS
MD_MAX72XX ledmat = MD_MAX72XX(LEDMAT_TYPE, LEDMAT_CS1, LEDMAT_MAX_DEVICES); // SPI hardware interface
SandSimulation sand_sim = SandSimulation(&ledmat);

#define SEG_CLK 2
#define SEG_DIO 3
TM1637Display seg_display(SEG_CLK, SEG_DIO);

// --- constants ---
const float disp_intensity = 0.001f;

const int spawn_xrange[] = {3,4};
const int spawn_y = 0;

const long spawn_interval = 250;
const long update_interval = 35;

void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(9600);
  sand_sim.init();
  sand_sim.setIntensity(disp_intensity);
}

unsigned long lastSpawn = 0;
unsigned long lastUpdate = 0;
void loop()
{
  if (millis() - lastUpdate >= update_interval || lastUpdate == 0)
  {
    lastUpdate = millis();
    sand_sim.updateField();
  }
  if (millis() - lastSpawn >= spawn_interval || lastSpawn == 0)
  {
    lastSpawn = millis();
    if (!sand_sim.spawnGrainInRegion(spawn_xrange, spawn_y)){
      sand_sim.resetField();
      sand_sim.spawnGrainInRegion(spawn_xrange, spawn_y);
    }
  }
}