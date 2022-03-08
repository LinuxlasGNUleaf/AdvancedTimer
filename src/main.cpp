#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define DEBUG 0 // Enable or disable (default) debugging output

#if DEBUG
#define PRINT(s, v)     \
  {                     \
    Serial.print(F(s)); \
    Serial.print(v);    \
  } // Print a string followed by a value (decimal)
#define PRINTX(s, v)      \
  {                       \
    Serial.print(F(s));   \
    Serial.print(v, HEX); \
  } // Print a string followed by a value (hex)
#define PRINTS(s)    \
  {                  \
    Serial.print(s); \
  } // Print a string
#else
#define PRINT(s, v)  // Print a string followed by a value (decimal)
#define PRINTX(s, v) // Print a string followed by a value (hex)
#define PRINTS(s)    // Print a string
#endif

// --------------------
// MD_MAX72xx hardware definitions and object
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
//
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define CLK_PIN 13  // or SCK
#define DATA_PIN 11 // or MOSI
#define CS_PIN 10   // or SS

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); // SPI hardware interface

// --- constants ---
const float disp_intensity = 0.0001f;

const int spawn_xpoints[] = {3,4};
const int spawn_y = 0;

const long spawn_delay = 75;
const long update_interval = 75;

const boolean is_steep = false;

// --- variables ---
boolean field[8][8] = {};
int active[64][2] = {};
int active_i = 0;

void resetField()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      field[i][j] = false;
    }
  }
}

boolean testForRoom(int i, boolean *sublayer)
{
  int x = active[i][0];
  int y = active[i][1];
  boolean found = false;
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
  char buf[40];
  sprintf(buf, "x: %d; y: %d sublayer: %d %d %d\n", x, y, sublayer[0], sublayer[1], sublayer[2]);
  PRINTS(buf);
  return found;
}

boolean dropGrainIntoField()
{
  int free_count = 0;
  for (int i = spawn_xpoints[0]; i <= spawn_xpoints[1]; i++){
    if (!field[spawn_y][i])
      free_count++;
  }
  if (free_count == 0)
    return false;
  
  int x = random(0,free_count+1);
  while (free_count >= 0)
  active[active_i][1] = spawn_y;
  active[active_i][spawn_y] = x;
  field[spawn_y][x] = true;
  active_i++;
  mx.setPoint(spawn_y, x, true);
  mx.update();
}

void lockGrain(int i)
{
  for (int j = i; j < active_i - 1; j++)
  {
    active[j][0] = active[j + 1][0];
    active[j][1] = active[j + 1][1];
  }
  active_i--;
}

void moveGrain(int active_i, boolean *sublayer)
{
  int x = active[active_i][0];
  int y = active[active_i][1];

  //remove grain from old position in field and on matrix
  mx.setPoint(y, x, false);
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
  mx.setPoint(y, x, true);
  field[y][x] = true;
}

void updateField()
{
  for (int i = 0; i < active_i; i++)
  {
    boolean sublayer[] = {false, false, false};
    if (testForRoom(i, sublayer))
    { // if there is room for the grain to fall
      moveGrain(i, sublayer);
    }
    else
    {
      char buf[35];
      sprintf(buf, "locking grain no. %d because it is stuck.", active_i);
      PRINTS(buf);
      lockGrain(i);
      i--;
    }
  }
  mx.update();
}

void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(9600);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY * disp_intensity);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear();
  resetField();
}

unsigned long lastSpawn = spawn_delay;
void loop()
{
  updateField();
  if (millis() - lastSpawn > spawn_delay || lastSpawn == 0)
  {
    lastSpawn = millis();
    dropGrainIntoField();
  }
  delay(update_interval);
}