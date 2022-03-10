#ifndef SAND_SIM
#define SAND_SIM

#include "MD_MAX72xx.h"
#define FIELD_SIZE 8

class SandSimulation
{ // a class to simulate and control an "hourglass" displayed on two LED 8x8-matrices
private:
    uint8_t bot_field[FIELD_SIZE];
    int bot_active[FIELD_SIZE*FIELD_SIZE][2];
    int bot_activeIndex;

    uint8_t top_field[FIELD_SIZE];
    int top_active[FIELD_SIZE*FIELD_SIZE][2];
    int top_activeIndex;
    MD_MAX72XX* ledmat;

    bool testForRoom(int, bool*);
    void lockGrain(int);
    void moveGrain(int, bool*);
public:
    SandSimulation(MD_MAX72XX*);
    ~SandSimulation();
    void init();
    void setIntensity(float);
    void resetField();
    void updateField();
    void testDims();
    bool spawnGrainInRegion(const int*, const int);
};

#endif