#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include "SandSimulation.h"

#define LEDMAT_TYPE MD_MAX72XX::FC16_HW
#define LEDMAT_MAX_DEVICES 2
#define LEDMAT_CLK 13  // or SCK
#define LEDMAT_DATA 11 // or MOSI
#define LEDMAT_CS 10   // or SS
MD_MAX72XX ledmat = MD_MAX72XX(LEDMAT_TYPE, LEDMAT_CS, LEDMAT_MAX_DEVICES); // SPI hardware interface

void setup(){
  ledmat.begin();
  ledmat.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
}

void loop(){
  ledmat.setPoint(0,15,false);
  ledmat.setPoint(0,0,true);
  ledmat.update();
  delay(200);
  for (int i = 1; i < 16; i++){
    ledmat.setPoint(0,i-1,false);
    ledmat.setPoint(0,i,true);
    ledmat.update();
    delay(200);
  }
}