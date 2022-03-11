#include "SandSimulation.h"
#include "Arduino.h"

/*
  *   HARDWARE LAYOUT
  *   
  * X/Y |  15 |  14 | 13 | 12 | 11 | 10 |  9 |  8 |
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
    *x = 15 - *x;
  }
  else
  {
    *x = 7 - *x;
    *y -= 8;
  }
}

SandSimulation::SandSimulation(const MD_MAX72XX::moduleType_t mat_type, const int *spi_bus, const int mat_count, uint16_t *constraints)
{
  this->mat_type = mat_type;
  this->spi_bus = spi_bus;
  this->mat_count = mat_count;
  this->constraints = constraints;
  this->active_i = 0;
  this->y_start = 0;
  this->y_stop = (FIELD_SIZE * 2) - 1;
  this->is_full = false;
  this->is_empty = false;
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

void SandSimulation::setBit(int x, int y, bool val)
{
  if (val)
    field[x] = (field[x] | (1 << y));
  else
    field[x] = (field[x] & (~(1 << y)));

  transformXY(&x, &y);
  ledmat->setPoint(y, x, val);
}

void SandSimulation::lockGrain(int index)
{
  for (int j = index; j < active_i - 1; j++)
  {
    active[j][0] = active[j + 1][0];
    active[j][1] = active[j + 1][1];
  }
  active_i--;
}

void SandSimulation::moveGrain(int index, bool *sublayer)
{
  int x = active[index][0];
  int y = active[index][1];
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
  active[index][0] = x;
  active[index][1] = y;
  setBit(x, y, true);
}

void SandSimulation::resetField(int y_start, int y_end)
{
  for (int x = 0; x < FIELD_SIZE; x++)
  {
    for (int y = y_start; y < y_end; y++)
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

void SandSimulation::setIntensity(float percent)
{
  ledmat->control(MD_MAX72XX::INTENSITY, MAX_INTENSITY * percent);
}

void SandSimulation::setYRange(int y_start, int y_stop)
{
  this->y_start = y_start;
  this->y_stop = y_stop;
  this->is_empty = false;
  this->is_full = false;
}

void SandSimulation::setUpdateIntervals(unsigned long ms_screen_update, unsigned long ms_grain_spawn)
{
  this->ms_screen_update = ms_screen_update;
  this->ms_grain_spawn = ms_grain_spawn;
}

void SandSimulation::init()
{
  ledmat = new MD_MAX72XX(mat_type, spi_bus[0], spi_bus[1], spi_bus[2], mat_count);

  ledmat->begin();
  ledmat->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  resetField();
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

void SandSimulation::spawnGrainInRegion(int x_start, int x_end)
{
  int free_count = 0;
  for (int xtest = x_start; xtest <= x_end; xtest++)
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
  int x = x_start - 1;
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
          is_full = false;
          return;
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

bool SandSimulation::testForRoom(int index, bool *sublayer)
{
  int x = active[index][0];
  int y = active[index][1];

  // "1" in the sublayer means: obstructed
  // "0" in the sublayer means: free
  sublayer[0] = getBit(field, x - 1, y + 1) || getBit(constraints, x - 1, y + 1);
  sublayer[1] = getBit(field, x, y + 1) || getBit(constraints, x, y + 1);
  sublayer[2] = getBit(field, x + 1, y + 1) || getBit(constraints, x + 1, y + 1);
  return !(sublayer[0] && sublayer[1] && sublayer[2]);
}

void SandSimulation::fillUpperHalf()
{
  this->setYRange(0, FIELD_SIZE);
  unsigned long last_update = 0;
  unsigned long last_spawn = 0;
  while (!is_full)
  {
    if (millis() - last_update >= ms_screen_update || last_update == 0)
    {
      uint8_t oldSREG = SREG;
      cli();

      last_update = millis();
      this->updateField();

      SREG = oldSREG;
    }
    if (millis() - last_spawn >= ms_grain_spawn || last_spawn == 0)
    {
      uint8_t oldSREG = SREG;
      cli();

      last_spawn = millis();
      this->spawnGrainInRegion(0, 7);
      SREG = oldSREG;
    }
  }
}

void SandSimulation::tickHourglass(unsigned long *last_screen, unsigned long *last_spawn)
{
  if (millis() - *last_screen >= ms_screen_update || last_screen == 0)
  {
    uint8_t oldSREG = SREG;
    cli();

    *last_screen = millis();
    updateField();

    SREG = oldSREG;
  }
  if (millis() - *last_spawn >= ms_grain_spawn || last_spawn == 0)
  {
    uint8_t oldSREG = SREG;
    cli();

    *last_spawn = millis();
    spawnGrainInRegion((FIELD_SIZE / 2) - 1, FIELD_SIZE / 2);
    removeGrainFromRegion(0, FIELD_SIZE - 1);

    SREG = oldSREG;
  }
}