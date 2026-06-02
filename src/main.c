#include <stdio.h>
#include "maze.h"
#include "mouse.h"
#include "API.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    struct Mouse* mouse = CreateMouse(16);
    mouse->SetUpMouse(mouse);

    SolveMaze(mouse);

    FreeMouse(mouse);
}
