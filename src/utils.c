#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


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

void PrintLocation(struct Location loc)
{
    const char* format = "Location: (%d, %d)";
    int len = snprintf(NULL, 0, format, loc.x, loc.y);
    char msg[len + 1];
    snprintf(msg, len + 1, format, loc.x, loc.y);
    debug_log(msg);
}
