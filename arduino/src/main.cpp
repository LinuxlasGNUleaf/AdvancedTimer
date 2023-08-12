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
  float progress;
  while (time_handler.timer_state != TIMER_STATE::FINISHED || display_handler.is_full == false)
  {
    time_handler.tick();
    progress = time_handler.calculateTimerProgress();
    display_handler.tick(progress);
  }
  time_handler.tick();

}