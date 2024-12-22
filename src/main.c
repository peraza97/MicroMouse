#include <stdio.h>
#include "maze.h"
#include "mouse.h"
#include "solver.h"
#include "API.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    struct Maze* maze = CreateMaze(16);
    maze->SetUp(maze);
    
    struct Mouse* mouse = CreateMouse(maze);

    debug_log("Running...");
    while (1) {
        PrintLocationFromLocation(mouse->location);
        PrintHeading(mouse->heading);

        Action nextMove = mouse->GetNextAction(mouse);
        PrintNextAction(nextMove);
        mouse->TakeAction(mouse, nextMove);
    }
}