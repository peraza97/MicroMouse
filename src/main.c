#include <stdio.h>
#include "mouse.h"
#include "solver.h"
#include "API.h"

char* PrintNextAction(Action nextMove)
{
    const char* format = "Action: %d";
    int len = snprintf(NULL, 0, format, nextMove);
    char msg[len + 1];
    snprintf(msg, len + 1, format, nextMove);
    debug_log(msg);
}

// You do not need to edit this file.
// This program just runs your solver and passes the choices
// to the simulator.
int main(int argc, char* argv[]) {
    int mazeSize = 16;

    struct Mouse* mouse = CreateMouse();

    debug_log("Running...");
    while (1) {
        mouse->PrintLocation(mouse);
        mouse->PrintHeading(mouse);

        Action nextMove = solver();
        PrintNextAction(nextMove);
        mouse->TakeAction(mouse, nextMove);
        
        switch(nextMove){
            case FORWARD:
                API_moveForward();
                break;
            case LEFT:
                API_turnLeft();
                break;
            case RIGHT:
                API_turnRight();
                break;
            case IDLE:
                break;
        }
    }
}