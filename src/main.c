#include <stdio.h>
#include "maze.h"
#include "mouse.h"
#include "solver.h"
#include "API.h"
#include "utils.h"

char* PrintNextAction(Action nextMove)
{
    const char* format = "Action: %d";
    int len = snprintf(NULL, 0, format, nextMove);
    char msg[len + 1];
    snprintf(msg, len + 1, format, nextMove);
    debug_log(msg);
}

int main(int argc, char* argv[]) {
    struct Maze* maze = CreateMaze(16);
    struct Mouse* mouse = CreateMouse(maze);

    debug_log("Running...");
    while (1) {
        PrintLocation(mouse->location);
        PrintHeading(mouse->heading);

        Action nextMove = mouse->GetNextAction(mouse);
        PrintNextAction(nextMove);
        mouse->TakeAction(mouse, nextMove);
    }
}