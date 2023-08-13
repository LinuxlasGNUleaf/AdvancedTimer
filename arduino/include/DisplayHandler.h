#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H

#include <config.h>

enum SIMULATION_STATE
{
    SIM_FILL,
    SIM_IDLE,
    SIM_RUNNING,
    SIM_RELOADING
};

class DisplayHandler
{
private:
    SIMULATION_STATE state;
    uint16_t matrix[MAT_WIDTH];
    uint16_t active[MAT_WIDTH];
    uint8_t active_index;
    MD_MAX72XX *mat_display;

    int y_top;
    int y_bottom;

    int free_count;
    int idle_count;

    unsigned long last_display_update;
    int i;

    bool getBit(uint16_t *arr, int x, int y);
    void setBit(uint16_t *arr, int x, int y, bool val);
    void setDisplayBit(int x, int y, bool val);
    void propagateField(bool inverted);
    void resetField();
    bool spawnGrain(int y);
    bool hasFreeSpot(int y);
    void removeFrom(int y1, int y2);
    bool isPosFree(int x, int y);
    void printField();

public:
    DisplayHandler();

    void init();
    void setup(SIMULATION_STATE new_state);
    void tick(double status);

    bool is_full;
    bool sim_idle;
};
#endif