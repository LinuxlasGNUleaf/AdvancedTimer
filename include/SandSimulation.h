#ifndef SAND_SIM
#define SAND_SIM

#include "MD_MAX72xx.h"

class SandSimulation
{
private:
    bool field[8][8];
    int active[64][2];
    int active_i;
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
    bool spawnGrainInRegion(const int*, const int);
};

#endif