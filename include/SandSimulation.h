#ifndef SAND_SIM
#define SAND_SIM

#include "MD_MAX72xx.h"
#define FIELD_SIZE 8


void transformXY(int *x, int *y);

class SandSimulation
{ // a class to simulate and control an "hourglass" displayed on two LED 8x8-matrices
private:
    uint16_t field[FIELD_SIZE];
    short active[FIELD_SIZE*FIELD_SIZE][2];
    int active_i;

    uint16_t* constraints;
    int y_start;
    int y_stop;

    MD_MAX72XX* ledmat;
    unsigned long ms_screen_update;
    unsigned long ms_grain_spawn;

    bool testForRoom(int index, bool *sublayer);
    void lockGrain(int index);
    void moveGrain(int index, bool *sublayer);
public:
    bool is_full;
    bool is_empty;

    SandSimulation(MD_MAX72XX *led_matrix, uint16_t *constraints, int ystart = 0, int ystop = (FIELD_SIZE * 2) - 1);

    bool getBit(uint16_t *field, int x, int y);
    void setBit(int x, int y, bool val);
    void resetField(int y_start=0, int y_end=FIELD_SIZE*2);
    void setIntensity(float percent);
    void setYRange(int y_start, int y_stop);
    void setUpdateIntervals(unsigned long ms_screen_update, unsigned long ms_grain_spawn);

    void init();
    void updateField();
    void testDims();

    void spawnGrainInRegion(int x_start = 0, int x_end = FIELD_SIZE-1);
    void removeGrainFromRegion(int y_start, int y_end);
    void fillUpperHalf();
    void tickHourglass(unsigned long *last_screen_update, unsigned long *last_grain_spawn);
};

#endif