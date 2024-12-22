#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include "maze.h"
#include "solver.h"

struct Mouse {
    struct Maze * maze;
    Heading heading;
    struct Location location;

    // Helper functions
    Action (*GetNextAction)(struct Mouse * mouse);
    void (*TakeAction)(struct Mouse * mouse, Action action);
};

struct Mouse * CreateMouse(struct Maze * maze);
void FreeMouse(struct Mouse * mouse);

Action GetNextAction(struct Mouse * mouse);
void TakeAction(struct Mouse * mouse, Action action);
int ComputeModulo(int a, int b);
struct Location GetSimulatorCoordinates(struct Location loc);

#endif