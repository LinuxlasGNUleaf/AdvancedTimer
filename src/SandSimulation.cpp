#include "SandSimulation.h"
#include "Arduino.h"

static bool getBit(byte *field, int row, int col)
{
  return (1 & (field[col] >> row));
}

static void setBit(byte *field, int row, int col, bool val)
{
  if (val)
    field[col] = (field[col] | (1 << row));
  else
    field[col] = (field[col] & (~(1 << row)));
}

SandSimulation::SandSimulation(MD_MAX72XX *led_matrix)
{
  ledmat = led_matrix;
  bot_activeIndex = 0;
}

SandSimulation::~SandSimulation()
{
  ledmat->control(MD_MAX72XX::SHUTDOWN, MD_MAX72XX::ON);
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
  for (int row = 0; row < FIELD_SIZE; row++)
  {
    for (int col = 0; col < FIELD_SIZE; col++)
    {
      setBit(bot_field, row, col, false);
    }
  }

  for (unsigned int i = 0; i < sizeof(bot_active) / sizeof(bot_active[0]); i++)
  {
    bot_active[i][0] = 0;
    bot_active[i][1] = 0;
  }
  ledmat->clear();
}

bool SandSimulation::testForRoom(int i, bool *sublayer)
{
  int x = bot_active[i][0];
  int y = bot_active[i][1];
  bool found = false;
  if (y == FIELD_SIZE - 1)
  { // reached bottom?
    return false;
  }

  if (!getBit(bot_field, y + 1, x))
  { // space directly downwards?
    sublayer[1] = true;
    found = true;
  }

  if (x > 0)
  {
    if (!getBit(bot_field, y + 1, x - 1))
    { // space down and left?
      sublayer[0] = true;
      found = true;
    }
  }

  if (x < FIELD_SIZE - 1)
  {
    if (!getBit(bot_field, y + 1, x + 1))
    { // space down and right?
      sublayer[2] = true;
      found = true;
    }
  }
  return found;
}

bool SandSimulation::spawnGrainInRegion(const int *xrange, const int y)
{
  int free_count = 0;
  for (int xtest = xrange[0]; xtest <= xrange[1]; xtest++)
  {
    if (!getBit(bot_field, y, xtest))
      free_count++;
  }

  if (free_count == 0)
    return false;

  int steps = random(1, free_count + 1);
  int x = xrange[0] - 1;
  while (steps > 0)
  {
    if (!getBit(bot_field, y, ++x))
    {
      steps--;
    }
  }

  bot_active[bot_activeIndex][0] = x;
  bot_active[bot_activeIndex][1] = y;
  bot_activeIndex++;
  
  setBit(bot_field, y, x, true);
  ledmat->setPoint(y, x, true);
  ledmat->update();
  return true;
}

void SandSimulation::lockGrain(int i)
{
  for (int j = i; j < bot_activeIndex - 1; j++)
  {
    bot_active[j][0] = bot_active[j + 1][0];
    bot_active[j][1] = bot_active[j + 1][1];
  }
  bot_activeIndex--;
}

void SandSimulation::moveGrain(int bot_activeIndex, bool *sublayer)
{
  int x = bot_active[bot_activeIndex][0];
  int y = bot_active[bot_activeIndex][1];

  //remove grain from old position in field and on matrix
  setBit(bot_field, y, x, false);

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
  bot_active[bot_activeIndex][0] = x;
  bot_active[bot_activeIndex][1] = y;
  setBit(bot_field, y, x, true);
}

void SandSimulation::updateField()
{
  for (int i = 0; i < bot_activeIndex; i++)
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

  ledmat->setBuffer(FIELD_SIZE - 1, FIELD_SIZE, bot_field);
  ledmat->setBuffer((2 * FIELD_SIZE) - 1, FIELD_SIZE, top_field);
  ledmat->update();
}