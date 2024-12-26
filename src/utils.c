#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "API.h"

char* HeadingsAbbreviation = "nesw";

struct Location* GetLocationFromCoordinates(int x, int y)
{
    struct Location * loc = (struct Location*)malloc(sizeof(struct Location));
    loc->x = x;
    loc->y = y;
    return loc;
}

void PrintNextAction(Action nextMove)
{
    const char* format = "Action: %d";
    int len = snprintf(NULL, 0, format, nextMove);
    char msg[len + 1];
    snprintf(msg, len + 1, format, nextMove);
    debug_log(msg);
}

void PrintHeading(Heading heading)
{
    switch (heading)
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

void PrintLocation(int x, int y)
{
    const char* format = "(%d, %d)";
    int len = snprintf(NULL, 0, format, x, y);
    char msg[len + 1];
    snprintf(msg, len + 1, format, x, y);
    debug_log(msg);
}

struct Location GetSimulatorCoordinatesFromLocation(struct Location loc)
{
    struct Location simLoc;
    simLoc.y = 15 - loc.x;
    simLoc.x = loc.y;
    return simLoc;
}

struct Location GetSimulatorCoordinates(int x, int y)
{
    struct Location simLoc;
    simLoc.y = 15 - x;
    simLoc.x = y;
    return simLoc;
}