#include "SandSimulation.h"
#include "Arduino.h"

/*
  *   HARDWARE LAYOUT
  *   
  * X/Y |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 |
  *  ---+----+----+----+----+----+----+----+----+
  *  7  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  6  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  5  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  4  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  3  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  2  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  1  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  0  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 
  * X/Y |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
  *  ---+----+----+----+----+----+----+----+----+
  *  0  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  1  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  2  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  3  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  4  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  5  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  6  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  7  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 
  *   SOFTWARE LAYOUT
  *   
  * X/Y |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
  *  ---+----+----+----+----+----+----+----+----+
  *  0  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  1  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  2  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  3  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  4  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  5  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  6  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  7  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 
  * X/Y |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
  *  ---+----+----+----+----+----+----+----+----+
  *  8  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  *  9  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 10  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 11  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 12  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 13  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 14  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
  * 15  | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ | ▢ |
*/

void transformXY(int *x, int *y)
{
  if (*y < 8)
  {
    *y = 7 - *y;
    *x += 8;
  }
  else
  {
    *x = 7 - *x;
    *y -= 8;
  }
}

bool SandSimulation::getBit(uint16_t *field, int x, int y)
{
  /**
   * gets the bit at the specified position if position is in field.
   * returns true otherwise for convenience purposes.
   * 
   * \param x software x-position
   * \param y software y-position
   * \return true if the bit is set, false if it isn't. Also returns true if position is undefined.
   */
  if (y < 0 || y >= y_stop || x < 0 || x >= FIELD_SIZE)
    return 1;
  return (1 & (field[x] >> y));
}

SandSimulation::SandSimulation(MD_MAX72XX *led_matrix, uint16_t *constraints, int ystart, int y_stop)
{
  this->ledmat = led_matrix;
  this->constraints = constraints;
  this->active_i = 0;
  this->y_start = ystart;
  this->y_stop = y_stop;
  this->is_full = false;
  this->is_empty = false;
}

void SandSimulation::init()
{
  ledmat->begin();
  ledmat->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  resetField();
}

void SandSimulation::setBit(int x, int y, bool val)
{
  if (val)
    field[x] = (field[x] | (1 << y));
  else
    field[x] = (field[x] & (~(1 << y)));

  transformXY(&x, &y);
  ledmat->setPoint(y, x, val);
}

void SandSimulation::setIntensity(float percent)
{
  ledmat->control(MD_MAX72XX::INTENSITY, MAX_INTENSITY * percent);
}

void SandSimulation::testDims()
{
  const int ms_delay = 50;
  for (int i = 0; i < FIELD_SIZE * 2; i++)
  {
    ledmat->setColumn(i, 0xff);
    ledmat->update();
    delay(ms_delay);
    ledmat->setColumn(i, false);
    ledmat->update();
  }
  for (int i = ledmat->getColumnCount() - 1; i >= 0; i--)
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
  for (int i = FIELD_SIZE - 1; i >= 0; i--)
  {
    ledmat->setRow(i, 0b11111111);
    ledmat->update();
    delay(ms_delay);
    ledmat->setRow(i, false);
    ledmat->update();
  }
}

void SandSimulation::resetField(int y1, int y2)
{
  for (int x = 0; x < FIELD_SIZE; x++)
  {
    for (int y = y1; y < y2; y++)
    {
      setBit(x, y, false);
    }
  }

  for (unsigned int i = 0; i < FIELD_SIZE * FIELD_SIZE; i++)
  {
    active[i][0] = 0;
    active[i][1] = 0;
  }
  ledmat->clear();
}

bool SandSimulation::testForRoom(int i, bool *sublayer)
{
  int x = active[i][0];
  int y = active[i][1];

  // "1" in the sublayer means: obstructed
  // "0" in the sublayer means: free
  sublayer[0] = getBit(field, x - 1, y + 1) || getBit(constraints, x - 1, y + 1);
  sublayer[1] = getBit(field, x, y + 1) || getBit(constraints, x, y + 1);
  sublayer[2] = getBit(field, x + 1, y + 1) || getBit(constraints, x + 1, y + 1);
  return !(sublayer[0] && sublayer[1] && sublayer[2]);
}

void SandSimulation::spawnGrainInRegion(const int *xrange)
{
  int free_count = 0;
  for (int xtest = xrange[0]; xtest <= xrange[1]; xtest++)
  {
    if (!getBit(field, xtest, y_start))
      free_count++;
  }

  if (free_count == 0)
  {
    is_full = true;
    return;
  }

  int count = random(1, free_count + 1);
  int x = xrange[0] - 1;
  while (count > 0)
  {
    if (!getBit(field, ++x, y_start))
    {
      count--;
    }
  }

  active[active_i][0] = x;
  active[active_i][1] = y_start;
  active_i++;

  setBit(x, y_start, true);
  ledmat->update();
}

void SandSimulation::removeGrainFromRegion(int y_start, int y_end)
{
  for (int y = y_start; y <= y_end; y++)
  {
    int count = 0;
    for (int x = 0; x < FIELD_SIZE; x++)
    {
      if (getBit(field, x, y))
        count++;
    }
    if (count == 0)
      continue;

    count = random(0, count);
    for (int x = 0; x < FIELD_SIZE; x++)
    {
      if (getBit(field, x, y))
      {
        if (count == 0)
        {
          setBit(x, y, false);
          ledmat->update();
          is_empty = false;
        }
        else
        {
          count--;
        }
      }
    }
  }
  is_empty = true;
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
  setBit(x, y, false);

  //advance y postion
  y++;

  // if field directly beneath is obstructed choose path based on available options
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
      x--;
    else
      x++;
  }
  active[active_i][0] = x;
  active[active_i][1] = y;
  setBit(x, y, true);
}

void SandSimulation::updateField()
{
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
}

void SandSimulation::setYRange(int y_start, int y_stop)
{
  this->y_start = y_start;
  this->y_stop = y_stop;
  this->is_empty = false;
  this->is_full = false;
}