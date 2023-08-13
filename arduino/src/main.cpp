#include <Arduino.h>
#include <config.h>
#include <DisplayHandler.h>
#include <TimerHandler.h>

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
  while (!display_handler.is_full || !display_handler.sim_idle || time_handler.timer_state != RUNNING)
  {
    display_handler.tick(0);
    time_handler.tick();
  }
  display_handler.setup(SIM_RUNNING);
}

float progress;
void loop()
{
  while (!display_handler.is_full || !display_handler.sim_idle || time_handler.timer_state == RUNNING)
  {
    progress = time_handler.calculateTimerProgress();
    display_handler.tick(progress);
    time_handler.tick();
  }
  while (time_handler.timer_state == FINISHED)
  {
    time_handler.tick();
  }
  time_handler.resetTimerHandler();
  display_handler.setup(SIM_RELOADING);
  while (!display_handler.is_full || !display_handler.sim_idle || time_handler.timer_state != RUNNING)
  {
    display_handler.tick(0);
    time_handler.tick();
  }
  display_handler.setup(SIM_RUNNING);
}