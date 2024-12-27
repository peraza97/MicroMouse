#include <stdlib.h>
#include "mouse.h"
#include "API.h"
#include "utils.h"

struct Mouse * CreateMouse(unsigned char mazeDimension) {
    struct Mouse *mouse = (struct Mouse*)malloc(sizeof(struct Mouse));
    mouse->maze = CreateMaze(mazeDimension);
    mouse->location.x = 15;
    mouse->location.y = 0;
    mouse->heading = NORTH;
    
    mouse->SetUpMouse = &SetUpMouse;
    mouse->GetNextAction = &GetNextAction;
    mouse->TakeAction = &TakeAction;
    mouse->CanMoveForward = &CanMoveForward;
    mouse->MoveForward = &MoveForward;
    mouse->TurnLeft = &TurnLeft;
    mouse->TurnRight = &TurnRight;
    return mouse;
}

void SetUpMouse(struct Mouse * mouse)
{
    // Set location and heading
    mouse->location.x = 15;
    mouse->location.y = 0;
    mouse->heading = NORTH;

    // Set up maze
    mouse->maze->SetUpMaze(mouse->maze);
}

void FreeMouse(struct Mouse * mouse)
{
    FreeMaze(mouse->maze);
    free(mouse);
}

Action GetNextAction(struct Mouse * mouse)
{
    return mouse->maze->GetNextMove(mouse->maze, mouse->location.x, mouse->location.y, mouse->heading);
}

void TakeAction(struct Mouse * mouse, Action action)
{
    switch (action)
    {
        case FORWARD:
            if (mouse->CanMoveForward(mouse) == 0)
            {
                mouse->maze->UpdateMaze(mouse->maze, mouse->location.x, mouse->location.y, mouse->heading);
            }
            else
            {
                mouse->MoveForward(mouse);
            }
            break;
        case LEFT:
            mouse->TurnLeft(mouse);
            break;
        case RIGHT:
            mouse->TurnRight(mouse);
            break;
        default:
            break;
    }
}

unsigned char CanMoveForward(struct Mouse * mouse)
{
    if (API_wallFront() == 1)
    {
        return 0;
    }
    return 1;
}

void MoveForward(struct Mouse * mouse)
{
    API_moveForward();
    if (mouse->heading % 2 == 0) //left right
    {
        mouse->location.x += (mouse->heading == NORTH ? -1 : 1);
    }
    else
    {
        mouse->location.y += (mouse->heading == EAST ? 1 : -1);
    }
}

void TurnLeft(struct Mouse * mouse)
{
    API_turnLeft();
    mouse->heading = ComputeModulo((int)mouse->heading - 1, 4);
}

void TurnRight(struct Mouse * mouse)
{
    API_turnRight();
    mouse->heading = ComputeModulo((int)mouse->heading + 1, 4);
}

int ComputeModulo(int a, int b)
{
    return ((a % b) + b) % b;
}