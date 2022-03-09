#include "SandSimulation.h"
#include "Arduino.h"

SandSimulation::SandSimulation(MD_MAX72XX *led_matrix)
{
    ledmat = led_matrix;
    active_i = 0;
}

SandSimulation::~SandSimulation()
{
  ledmat->control(MD_MAX72XX::SHUTDOWN,MD_MAX72XX::ON);
}

void SandSimulation::init()
{
  ledmat->begin();
  ledmat->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  resetField();
}

void SandSimulation::setIntensity(float percent)
{
  ledmat->control(MD_MAX72XX::INTENSITY, MAX_INTENSITY * percent);
}

void SandSimulation::resetField()
{
  for (int y = 0; y < 8; y++)
  {
    for (int x = 0; x < 8; x++)
    {
      field[y][x] = false;
    }
  }

  for (unsigned int i = 0; i < sizeof(active)/sizeof(active[0]); i++){
    active[i][0] = 0;
    active[i][1] = 0;
  }
  ledmat->clear();
}

bool SandSimulation::testForRoom(int i, bool *sublayer)
{
  int x = active[i][0];
  int y = active[i][1];
  bool found = false;
  if (y == 7)
  { // reached bottom?
    return false;
  }

  if (!field[y + 1][x])
  { // space directly downwards?
    sublayer[1] = true;
    found = true;
  }

  if (x > 0)
  {
    if (!field[y + 1][x - 1])
    { // space down and left?
      sublayer[0] = true;
      found = true;
    }
  }

  if (x < 7)
  {
    if (!field[y + 1][x + 1])
    { // space down and right?
      sublayer[2] = true;
      found = true;
    }
  }
  return found;
}

bool SandSimulation::spawnGrainInRegion(const int* xrange, const int y)
{
  uint8_t oldSREG = SREG;
  cli();
  
  int free_count = 0;
  for (int xtest = xrange[0]; xtest <= xrange[1]; xtest++){
    if (!field[y][xtest])
      free_count++;
  }

  if (free_count == 0)
    return false;

  int steps = random(1,free_count+1);
  int x = xrange[0]-1;
  while (steps > 0){
    if (!field[y][++x]){
      steps--;
    }
  }
  active[active_i][0] = x;
  active[active_i][1] = y;
  field[y][x] = true;
  active_i++;
  ledmat->setPoint(y, x, true);
  ledmat->update();

  SREG = oldSREG;
  return true;
}

void SandSimulation::lockGrain(int i)
{
  for (int j = i; j < active_i - 1; j++)
  {
    active[j][0] = active[j + 1][0];
    active[j][1] = active[j + 1][1];
  }
  active_i--;
}

void SandSimulation::moveGrain(int active_i, bool *sublayer)
{
  int x = active[active_i][0];
  int y = active[active_i][1];

  //remove grain from old position in field and on matrix
  ledmat->setPoint(y, x, false);
  field[y][x] = false;

  //advance y postion
  y++;

  // if field directly beneath is unobstructed, move straight down
  // else, choose path based on available options
  if (!sublayer[1])
  {
    if (sublayer[0] && sublayer[2])
    {
      if (random(0, 2) == 1)
        x--;
      else
        x++;
    }
    else if (sublayer[0])
    {
      x--;
    }
    else
    {
      x++;
    }
  }
  active[active_i][0] = x;
  active[active_i][1] = y;
  ledmat->setPoint(y, x, true);
  field[y][x] = true;
}

void SandSimulation::updateField()
{  
  uint8_t oldSREG = SREG;
  cli();
  for (int i = 0; i < active_i; i++)
  {
    bool sublayer[] = {false, false, false};
    if (testForRoom(i, sublayer))
    { // if there is room for the grain to fall
      moveGrain(i, sublayer);
    }
    else
    {
      lockGrain(i);
      i--;
    }
  }
  ledmat->update();
  
  SREG = oldSREG;
}