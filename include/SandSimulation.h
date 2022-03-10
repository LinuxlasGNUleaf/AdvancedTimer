#ifndef SAND_SIM
#define SAND_SIM

#include "MD_MAX72xx.h"
#define FIELD_SIZE 8


void transformXY(int *, int*);

class SandSimulation
{ // a class to simulate and control an "hourglass" displayed on two LED 8x8-matrices
private:
    uint16_t field[FIELD_SIZE];
    short active[FIELD_SIZE*FIELD_SIZE][2];
    int active_i;
    MD_MAX72XX* ledmat;
    uint16_t* constraints;
    unsigned long ms_screen_update;
    unsigned long ms_grain_spawn;

    bool testForRoom(int, bool*);
    void lockGrain(int);
    void moveGrain(int, bool*);
public:
    bool getBit(uint16_t*, int, int);
    int y_start;
    int y_stop;
    bool is_full;
    bool is_empty;
    SandSimulation(MD_MAX72XX*, uint16_t*, int ystart = 0, int ystop = (FIELD_SIZE * 2) - 1);
    void init();
    void setIntensity(float);
    void resetField(int y1=0, int y2=FIELD_SIZE*2);
    void updateField();
    void testDims();
    void setBit(int, int, bool);
    void spawnGrainInRegion(int x_start = 0, int x_end = FIELD_SIZE-1);
    void removeGrainFromRegion(int, int);
    void setYRange(int, int);
    void setUpdateIntervals(unsigned long, unsigned long);
    void fillUpperHalf();
    void tickHourglass(unsigned long *last_screen_update, unsigned long *last_grain_spawn);
};

#endif