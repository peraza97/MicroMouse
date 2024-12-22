#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "mouse.h"
#include "API.h"

struct Mouse * CreateMouse(struct Maze * maze) {
    struct Mouse *mouse = malloc(sizeof (struct Mouse));
    mouse->maze = maze;
    mouse->location.x = 0;
    mouse->location.y = 0;
    mouse->heading = NORTH;
    mouse->PrintLocation = &PrintLocation;
    mouse->PrintHeading = &PrintHeading;
    mouse->GetNextAction = &solver;
    mouse->TakeAction = &TakeAction;
    return mouse;
}

void FreeMouse(struct Mouse * mouse)
{
    free(mouse);
}

void PrintHeading(struct Mouse * mouse)
{
    switch (mouse->heading)
    {
    case NORTH:
        debug_log("NORTH");
        break;
    case WEST:
        debug_log("WEST");
        break;
    case SOUTH:
        debug_log("SOUTH");
        break;
    case EAST:
        debug_log("EAST");
        break;
    default:
        break;
    }
}

void PrintLocation(struct Mouse * mouse)
{
    const char* format = "Location: (%d, %d)";
    int len = snprintf(NULL, 0, format, mouse->location.y, mouse->location.x);
    char msg[len + 1];
    snprintf(msg, len + 1, format, mouse->location.y, mouse->location.x);
    debug_log(msg);
}

Action GetNextAction(struct Mouse * mouse)
{
    return solver();
}

void TakeAction(struct Mouse * mouse, Action action)
{
    switch (action)
    {
        case FORWARD:
            API_moveForward();
            if (mouse->heading % 2 == 0) //left right
            {
                mouse->location.x += (mouse->heading == NORTH ? 1 : -1);
            }
            else
            {
                mouse->location.y += (mouse->heading == EAST ? 1 : -1);
            }
            break;
        case LEFT:
            API_setWall(mouse->location.y, mouse->location.x, HeadingsAbbreviation[mouse->heading]);
            API_turnLeft();
            mouse->heading = ComputeModulo((int)mouse->heading - 1, 4);
            break;
        case RIGHT:
            API_setWall(mouse->location.y, mouse->location.x, HeadingsAbbreviation[mouse->heading]);
            API_turnRight();
            mouse->heading = ComputeModulo((int)mouse->heading + 1, 4);
            break;
        default:
            break;
    }
}

int ComputeModulo(int a, int b)
{
    return ((a % b) + b) % b;
}


struct Location GetSimulatorCoordinates(int x, int y)
{
    struct Location simLoc;
    simLoc.x = y + 9;
    simLoc.y = 15 - x + 9;
}