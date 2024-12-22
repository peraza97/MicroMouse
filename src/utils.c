#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "API.h"

char* HeadingsAbbreviation = "nesw";

char* PrintNextAction(Action nextMove)
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

void PrintLocationFromLocation(struct Location loc)
{
    const char* format = "Location: (%d, %d)";
    int len = snprintf(NULL, 0, format, loc.x, loc.y);
    char msg[len + 1];
    snprintf(msg, len + 1, format, loc.x, loc.y);
    debug_log(msg);
}

void PrintLocation(int x, int y)
{
    const char* format = "Location: (%d, %d)";
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