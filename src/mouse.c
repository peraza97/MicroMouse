#include <stdlib.h>
#include <stdio.h>
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
    mouse->DebugInfo = &DebugInfo;
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
                mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, mouse->heading);
                mouse->maze->UpdateMaze(mouse->maze, mouse->location.x, mouse->location.y);
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
    switch (mouse->heading)
    {
        case NORTH:
            mouse->location.x += -1;
            break;
        case EAST:
            mouse->location.y += 1;
            break;
        case SOUTH:
            mouse->location.x += 1;
            break;
        case WEST:
            mouse->location.y += -1;
            break;
        default:
            break;
    }
}

void TurnLeft(struct Mouse * mouse)
{
    API_turnLeft();
    mouse->heading = ComputeModulo((int)mouse->heading - 2, NUM_HEADINGS);
}

void TurnRight(struct Mouse * mouse)
{
    API_turnRight();
    mouse->heading = ComputeModulo((int)mouse->heading + 2, NUM_HEADINGS);
}

void DebugInfo(struct Mouse * mouse)
{
    const char* format = "Mouse State: (%d, %d). %s";
    char * heading = GetHeadingStr(mouse->heading);
    int len = snprintf(NULL, 0, format, mouse->location.x, mouse->location.y, heading);
    char msg[len + 1];
    snprintf(msg, len + 1, format, mouse->location.x, mouse->location.y, heading);
    debug_log(msg);
    debug_log("__________________");
}

int ComputeModulo(int a, int b)
{
    return ((a % b) + b) % b;
}