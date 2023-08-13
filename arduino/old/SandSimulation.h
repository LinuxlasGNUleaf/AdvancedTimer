#ifndef SANDSIMULATION_H
#define SANDSIMULATION_H

#include <MD_MAX72xx.h>
#include <TimerHandler.h>

class SandSimulation
{ // a class to simulate and control an "hourglass" displayed on two LED 8x8-matrices
private:
    uint16_t field[MAT_WIDTH];
    short active[MAT_WIDTH * MAT_WIDTH][2];
    int active_i;

    int y_start;
    int y_stop;

    MD_MAX72XX *ledmat;

    unsigned long ms_screen_update;
    unsigned long ms_grain_spawn;

    bool testForRoom(int index, bool *sublayer);
    void lockGrain(int index);
    void moveGrain(int index, bool *sublayer);

public:
    bool is_full;
    bool is_empty;

    SandSimulation();

    bool getBit(uint16_t *field, int x, int y);
    void setBit(int x, int y, bool val);
    void resetField(int y_start = 0, int y_end = MAT_WIDTH * 2);
    void setYRange(int y_start, int y_stop);
    void setUpdateIntervals(unsigned long ms_screen_update, unsigned long ms_grain_spawn);
    unsigned long calculateHourglassSpawnTime(unsigned long minutes);

    void init();
    void updateField();
    void testDims();

    void spawnGrainInRegion(int x_start = 0, int x_end = MAT_WIDTH - 1);
    void removeGrainFromRegion(int y_start, int y_end);
    void tickFillUpperHalf(unsigned long *last_update, unsigned long *last_grain_spawn);
    void tickHourglass(unsigned long *last_update, unsigned long *last_grain_spawn, TIMER_STATE timer_state);
};

#endif