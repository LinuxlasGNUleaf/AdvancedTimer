#include <DisplayHandler.h>

DisplayHandler::DisplayHandler()
{
    state = SIM_IDLE;
    last_display_update = 0;
}

bool DisplayHandler::getBit(uint16_t *arr, int x, int y)
{
    /**
     * gets the bit at the specified position if position is in field.
     * returns true otherwise for convenience purposes.
     *
     * \param arr field to get the data point from
     * \param x software x-position
     * \param y software y-position
     * \return true if the bit is set, false if it isn't.
     */
    return (1 & (arr[x] >> y));
}

void DisplayHandler::setBit(uint16_t *arr, int x, int y, bool val)
{
    /**
     * sets the bit at the specified position to val.
     *
     * \param arr field to set the data point in
     * \param x software x-position
     * \param y software y-position
     * \param val state of the bit to set
     */
    if (val)
        arr[x] |= (1 << y);
    else
        arr[x] &= ~(1 << y);
}

void DisplayHandler::setDisplayBit(int a, int b, bool val)
{
    int x, y;
    if (b <= 7)
    {
        y = 15 - a;
        x = b;
    }
    else
    {
        y = 7 - a;
        x = b - 8;
    }
    mat_display->setPoint(x, y, val);
}

bool DisplayHandler::isPosFree(int x, int y)
{
    if (x < 0 || x >= MAT_WIDTH)
        return false;
    if (y < min(y_bottom, y_top) || y > max(y_bottom, y_top))
        return false;
    return !getBit(matrix, x, y) && !getBit(MAT_CONSTRAINTS, x, y);
}

void DisplayHandler::propagateField(bool inversed)
{
    sim_idle = true;
    int dir = inversed ? -1 : 1;
    int y;
    // y = MAT_WIDTH * MAT_MODULE_COUNT - 1
    for (int a = 0; a <= abs(y_bottom - y_top); a++)
    {
        if (inversed)
        {
            // inversed gravity
            y = a;
        }
        else
        {
            // normal gravity
            y = y_bottom - a;
        }
        for (int x = 0; x < MAT_WIDTH; x++)
        {
            // check if grain is active
            if (!getBit(active, x, y))
                continue;
            sim_idle = false;
            // remove grain from active matrix
            setBit(active, x, y, false);

            // check whether position is final, if so
            if (!isPosFree(x - 1, y + dir) && !isPosFree(x, y + dir) && !isPosFree(x + 1, y + dir))
            {
                continue;
            }

            // remove grain from old position in field and on matrix
            setBit(matrix, x, y, false);
            setDisplayBit(x, y, false);

            // advance y postion
            y += dir;

            // if field directly "infront" is obstructed choose path based on available options
            if (!isPosFree(x, y))
            {
                // if both sides of are free, flip a coin
                if (isPosFree(x - 1, y) && isPosFree(x + 1, y))
                {
                    if (random(0, 2) == 1)
                        x++;
                    else
                        x--;
                }
                // else chose the free side
                else if (isPosFree(x - 1, y))
                    x--;
                else
                    x++;
            }
            setBit(matrix, x, y, true);
            setDisplayBit(x, y, true);
            setBit(active, x, y, true);
        }
    }
    mat_display->update();
}

void DisplayHandler::resetField()
{
    for (int y = 0; y < MAT_WIDTH * MAT_MODULE_COUNT; y++)
    {
        for (int x = 0; x < MAT_WIDTH; x++)
        {
            setBit(matrix, x, y, false);
            setBit(active, x, y, false);
        }
    }
    mat_display->clear();
}

void DisplayHandler::init()
{
    mat_display = new MD_MAX72XX(MAT_TYPE, MAT_SPI_BUS[0], MAT_SPI_BUS[1], MAT_SPI_BUS[2], MAT_MODULE_COUNT);

    mat_display->begin();
    mat_display->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    mat_display->control(MD_MAX72XX::INTENSITY, MAT_BRIGHTNESS);
    resetField();
    mat_display->update();
}

void DisplayHandler::printField()
{
#if DEBUG
    SPRINTLN(" MATRIX\t|\tACTIVE:");
    for (int y = 0; y < MAT_WIDTH * MAT_MODULE_COUNT; y++)
    {
        for (int x = 0; x < MAT_WIDTH; x++)
        {
            SPRINT(getBit(matrix, x, y));
        }
        SPRINT("\t");
        for (int x = 0; x < MAT_WIDTH; x++)
        {
            SPRINT(getBit(active, x, y));
        }
        SPRINTLN();
    }
    SPRINTLN("\n");
#endif
}

bool DisplayHandler::spawnGrain(int y)
{
    if (!hasFreeSpot(y))
    {
        is_full = true;
        return false;
    }

    while (true)
    {
        int x = rand() % MAT_WIDTH;
        if (isPosFree(x, y))
        {
            setBit(matrix, x, y, true);
            setBit(active, x, y, true);
            setDisplayBit(x, y, true);
            free_count--;
            is_full = !hasFreeSpot(y);
            return true;
        }
    }
}

bool DisplayHandler::hasFreeSpot(int y)
{
    for (int x = 0; x < MAT_WIDTH; x++)
    {
        if (isPosFree(x, y))
            return true;
    }
    return false;
}

void DisplayHandler::tick(double timer_status)
{
    if (!INTERVAL_PASSED(last_display_update, MAT_DISP_UPDATE_INTERVAL, millis()))
        return;
    last_display_update = millis();

    bool prop_inversed = false;
    bool spawn = false, remove = false, spawn_res = false;
    int spawn_row = 0, remove_top = 0, remove_bottom = 0;
    float status_diff_current, status_diff_next;

    switch (state)
    {
    case SIM_IDLE:
        return;

    case SIM_FILL:
        spawn_row = 0;
        if (idle_count % MAT_GRAIN_SPAWN_MULT == 0)
            spawn = true;
        idle_count++;
        break;

    case SIM_RUNNING:
        spawn_row = MAT_WIDTH;
        status_diff_current = abs((float(free_count) / MAT_FREE) - (1 - timer_status));
        status_diff_next = abs((float(free_count - 1) / MAT_FREE) - (1 - timer_status));

        if (status_diff_current > status_diff_next)
        {
            spawn = true;
            remove = true;
            remove_top = 0;
            remove_bottom = MAT_WIDTH - 1;
        }
        break;

    case SIM_RELOADING:
        spawn_row = MAT_WIDTH - 1;
        if (idle_count % MAT_GRAIN_SPAWN_MULT == 0)
        {
            spawn = true;
            remove = true;
            remove_top = MAT_WIDTH * 2 - 1;
            remove_bottom = MAT_WIDTH;
        }
        idle_count++;
        prop_inversed = true;
        break;

    default:
        break;
    }

    propagateField(prop_inversed);
    if (spawn)
    {
        spawn_res = spawnGrain(spawn_row);
        sim_idle = false;
    }
    if (remove && spawn_res)
        removeFrom(remove_top, remove_bottom);

    mat_display->update();
}

void DisplayHandler::setup(SIMULATION_STATE new_state)
{
    switch (new_state)
    {
    case SIM_IDLE:
        return;

    case SIM_FILL:
        i = 0;
        y_top = 0;
        y_bottom = MAT_WIDTH - 1;
        idle_count = 0;
        break;

    case SIM_RUNNING:
        y_top = MAT_WIDTH;
        y_bottom = 2 * MAT_WIDTH - 1;
        free_count = MAT_FREE;
        break;

    case SIM_RELOADING:
        y_top = MAT_WIDTH - 1;
        y_bottom = 0;
        free_count = MAT_FREE;
        break;

    default:
        break;
    }

    state = new_state;
    is_full = false;
}

void DisplayHandler::removeFrom(int y1, int y2)
{
    for (int y = y1; (y1 < y2 ? y <= y2 : y >= y2); (y1 < y2 ? y++ : y--))
    {
        // see if there are any points to remove in this row
        bool found = false;
        for (int x = 0; x < MAT_WIDTH; x++)
        {
            if (getBit(matrix, x, y))
            {
                found = true;
                break;
            }
        }
        // if not, continue to the next
        if (!found)
            continue;

        while (true)
        {
            int x = rand() % MAT_WIDTH;
            if (getBit(matrix, x, y))
            {
                setBit(matrix, x, y, false);
                setDisplayBit(x, y, false);
                return;
            }
        }
    }
}