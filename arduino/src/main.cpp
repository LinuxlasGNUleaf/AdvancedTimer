#include <Arduino.h>
#include <config.h>
#include <SandSimulation.h>
#include <TimerHandler.h>


TimerHandler time_handler = TimerHandler();
SandSimulation sand_sim = SandSimulation();

void tickPosition()
{
  time_handler.enc->tick(); // just call tick() to check the timer_state.
}

void setup()
{
  randomSeed(analogRead(0));

  #if DEBUG
    Serial.begin(SERIAL_SPEED);
  #endif
  // initialize objects & attach interrupts
  time_handler.init(tickPosition);
  sand_sim.init();

  // fill upper half of hourglass
  sand_sim.setUpdateIntervals(MAT_DISP_UPDATE_INTERVAL, MAT_GRAIN_SPAWN_INTERVAL);
  sand_sim.setYRange(0, MAT_WIDTH);

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
  while (time_handler.timer_state == SELECT_TIME)
  {
    time_handler.tick();
  }

  sand_sim.setUpdateIntervals(MAT_DISP_UPDATE_INTERVAL, 50);
  sand_sim.setYRange(MAT_WIDTH, 2 * MAT_WIDTH);

  while (true)
  {
    sand_sim.tickHourglass(&last_update, &last_spawn, time_handler.timer_state);
    time_handler.tick();
  }
}