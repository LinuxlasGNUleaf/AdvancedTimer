#include <Arduino.h>
#include <config.h>
#include <SandSimulation.h>
#include <DisplayHandler.h>

TimerHandler time_handler = TimerHandler();
DisplayHandler display_handler = DisplayHandler();

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
  display_handler.init();

  // fill upper half of hourglass
  display_handler.setup(SIM_FILL);
  while (!display_handler.is_full || time_handler.timer_state != RUNNING)
  {
    display_handler.tick(0);
    time_handler.tick();
  }
  display_handler.setup(SIM_RUNNING);
}

unsigned long last_update = 0;
unsigned long last_spawn = 0;

void loop()
{
  // sand_sim.setUpdateIntervals(MAT_DISP_UPDATE_INTERVAL, 50);
  // sand_sim.setYRange(MAT_WIDTH, 2 * MAT_WIDTH);

  float progress;
  while (true)
  {
    // sand_sim.tickHourglass(&last_update, &last_spawn, time_handler.timer_state);
    time_handler.tick();
    progress = time_handler.calculateTimerProgress();
    display_handler.tick(progress);
    //SPRINTLN(progress);
  }
}