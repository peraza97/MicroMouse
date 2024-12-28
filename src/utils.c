#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "API.h"

struct Location* GetLocationFromCoordinates(int x, int y)
{
    struct Location * loc = (struct Location*)malloc(sizeof(struct Location));
    loc->x = x;
    loc->y = y;
    return loc;
}

char GetHeadingAbbreviation(Heading heading)
{
    switch (heading)
    {
        case NORTH:
            return 'n';
        case EAST:
            return 'e';
        case SOUTH:
            return 's';
        case WEST:
            return 'w';
        default:
            return '?';
    }
}

char* GetHeadingStr(Heading heading)
{
    switch (heading)
    {
    case NORTH:
        return "NORTH";
    case WEST:
        return "WEST";
    case SOUTH:
        return "SOUTH";
    case EAST:
        return "EAST";
    default:
        return "?";
    }
}

void PrintNextAction(Action nextMove)
{
    const char* format = "Action: %d";
    int len = snprintf(NULL, 0, format, nextMove);
    char msg[len + 1];
    snprintf(msg, len + 1, format, nextMove);
    debug_log(msg);
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