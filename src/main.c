#include <stdio.h>
#include "maze.h"
#include "mouse.h"
#include "solver.h"
#include "API.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    struct Mouse* mouse = CreateMouse(16);
    mouse->SetUpMouse(mouse);
    
    debug_log("Running...");
    Action nextMove = IDLE;
    do 
    {
        nextMove = mouse->GetNextAction(mouse);
        mouse->TakeAction(mouse, nextMove);
    } while (nextMove != IDLE);

    FreeMouse(mouse);
}