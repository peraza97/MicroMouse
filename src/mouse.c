#include <stdlib.h>
#include "solver.h"
#include "mouse.h"
#include "API.h"
#include "utils.h"

struct Mouse * CreateMouse(struct Maze * maze) {
    struct Mouse *mouse = malloc(sizeof (struct Mouse));
    mouse->maze = maze;
    mouse->location.x = 15;
    mouse->location.y = 0;
    mouse->heading = NORTH;
    mouse->GetNextAction = &solver;
    mouse->TakeAction = &TakeAction;
    return mouse;
}

void FreeMouse(struct Mouse * mouse)
{
    free(mouse);
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
                mouse->location.x += (mouse->heading == NORTH ? -1 : 1);
            }
            else
            {
                mouse->location.y += (mouse->heading == EAST ? 1 : -1);
            }
            break;
        case LEFT:
            API_turnLeft();
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, HeadingsAbbreviation[mouse->heading]);
            mouse->heading = ComputeModulo((int)mouse->heading - 1, 4);
            break;
        case RIGHT:
            API_turnRight();
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, HeadingsAbbreviation[mouse->heading]);
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