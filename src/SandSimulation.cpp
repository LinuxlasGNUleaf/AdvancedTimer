#include "SandSimulation.h"
#include "Arduino.h"

/*
x/col:  7   6   5   4   3   2   1   0     y/row:
                                      
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   0
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   1
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   2
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   3
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   4
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   5
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   6
      | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |   7

*/

static bool getBit(byte *field, int x, int y)
{
  /**
   * gets the bit at the specified position if position is in field.
   * returns true otherwise for convenience purposes.
   * 
   * \param field
   * \param x
   * \param y
   * \return true if the bit is set, false if it isn't. Also returns true if position is undefined.
   */
  if (y < 0 || y >= FIELD_SIZE || x < 0 || x >= FIELD_SIZE)
    return 1;
  return (1 & (field[x] >> y));
}

static void setBit(MD_MAX72XX *led_matrix, byte *field, int x, int y, bool val)
{
  if (val)
    field[x] = (field[x] | (1 << y));
  else
    field[x] = (field[x] & (~(1 << y)));

  led_matrix->setPoint(y, x, val);
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

void SandSimulation::testDims()
{
  const int ms_delay = 50;
  for (int i = 0; i < (int) ledmat->getColumnCount(); i++)
  {
    ledmat->setColumn(i, 0xff);
    ledmat->update();
    delay(ms_delay);
    ledmat->setColumn(i, false);
    ledmat->update();
  }
  for (int i = ledmat->getColumnCount()-1; i >= 0; i--)
  {
    ledmat->setColumn(i, 0xff);
    ledmat->update();
    delay(ms_delay);
    ledmat->setColumn(i, false);
    ledmat->update();
  }
  for (int i = 0; i < FIELD_SIZE; i++)
  {
    ledmat->setRow(i, 0b11111111);
    ledmat->update();
    delay(ms_delay);
    ledmat->setRow(i, false);
    ledmat->update();
  }
  for (int i = FIELD_SIZE-1; i >= 0; i--)
  {
    ledmat->setRow(i, 0b11111111);
    ledmat->update();
    delay(ms_delay);
    ledmat->setRow(i, false);
    ledmat->update();
  }
}

void SandSimulation::resetField()
{
  for (int x = 0; x < FIELD_SIZE; x++)
  {
    for (int y = 0; y < FIELD_SIZE; y++)
    {
      setBit(ledmat, bot_field, x, y, false);
    }
  }

  for (unsigned int i = 0; i < FIELD_SIZE * FIELD_SIZE; i++)
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

  // "1" in the sublayer means: obstructed
  // "0" in the sublayer means: free
  sublayer[0] = getBit(bot_field, x + 1, y + 1);
  sublayer[1] = getBit(bot_field, x, y + 1);
  sublayer[2] = getBit(bot_field, x - 1, y + 1);

  return !(sublayer[0] && sublayer[1] && sublayer[2]);
}

bool SandSimulation::spawnGrainInRegion(const int *xrange, const int ystart)
{
  int free_count = 0;
  for (int xtest = xrange[0]; xtest <= xrange[1]; xtest++)
  {
    if (!getBit(bot_field, xtest, ystart))
      free_count++;
  }

  if (free_count == 0)
    return false;

  int steps = random(1, free_count + 1);
  int x = xrange[0] - 1;
  while (steps > 0)
  {
    if (!getBit(bot_field, ++x, ystart))
    {
      steps--;
    }
  }

  bot_active[bot_activeIndex][0] = x;
  bot_active[bot_activeIndex][1] = ystart;
  bot_activeIndex++;

  setBit(ledmat, bot_field, x, ystart, true);
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
  setBit(ledmat, bot_field, x, y, false);

  //advance y postion
  y++;

  // if field directly beneath is unobstructed, move straight down
  // else, choose path based on available options
  if (sublayer[1])
  {
    if (!(sublayer[0] || sublayer[2]))
    {
      if (random(0, 2) == 1)
        x++;
      else
        x--;
    }
    else if (!sublayer[0])
      x++;
    else
      x--;
  }
  bot_active[bot_activeIndex][0] = x;
  bot_active[bot_activeIndex][1] = y;
  setBit(ledmat, bot_field, x, y, true);
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
  ledmat->update();
}