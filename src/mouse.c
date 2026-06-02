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
    mouse->CheckWallLeft = &CheckWallLeft;
    mouse->CheckWallRight = &CheckWallRight;
    mouse->MoveForward = &MoveForward;
    mouse->TurnLeft = &TurnLeft;
    mouse->TurnRight = &TurnRight;
    mouse->TurnLeft45 = &TurnLeft45;
    mouse->TurnRight45 = &TurnRight45;
    mouse->DebugMouseState = &DebugMouseState;
    mouse->SenseWalls = &SenseWalls;
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
        {
            Heading h = mouse->heading;
            if (h == NORTHEAST || h == SOUTHEAST || h == SOUTHWEST || h == NORTHWEST)
            {
                if (CanMoveDiagonally(mouse->maze, mouse->location.x, mouse->location.y, h))
                {
                    mouse->MoveForward(mouse);
                }
                else
                {
                    mouse->maze->UpdateMaze(mouse->maze, mouse->location.x, mouse->location.y);
                }
            }
            else
            {
                if (mouse->CanMoveForward(mouse) == 0)
                {
                    mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, mouse->heading);
                    mouse->maze->UpdateMaze(mouse->maze, mouse->location.x, mouse->location.y);
                }
                else
                {
                    mouse->MoveForward(mouse);
                }
            }
            break;
        }
        case LEFT:
            mouse->TurnLeft(mouse);
            break;
        case RIGHT:
            mouse->TurnRight(mouse);
            break;
        case LEFT45:
            mouse->TurnLeft45(mouse);
            break;
        case RIGHT45:
            mouse->TurnRight45(mouse);
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

void CheckWallLeft(struct Mouse * mouse)
{
    if (API_wallLeft())
    {
        Heading wallDir = ComputeModulo((int)mouse->heading - 2, NUM_HEADINGS);
        if (wallDir == NORTH || wallDir == EAST || wallDir == SOUTH || wallDir == WEST)
        {
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, wallDir);
        }
    }
}

void CheckWallRight(struct Mouse * mouse)
{
    if (API_wallRight())
    {
        Heading wallDir = ComputeModulo((int)mouse->heading + 2, NUM_HEADINGS);
        if (wallDir == NORTH || wallDir == EAST || wallDir == SOUTH || wallDir == WEST)
        {
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, wallDir);
        }
    }
}

void MoveForward(struct Mouse * mouse)
{
    switch (mouse->heading)
    {
        case NORTH:
            API_moveForward();
            mouse->location.x += -1;
            break;
        case EAST:
            API_moveForward();
            mouse->location.y += 1;
            break;
        case SOUTH:
            API_moveForward();
            mouse->location.x += 1;
            break;
        case WEST:
            API_moveForward();
            mouse->location.y += -1;
            break;
        case NORTHEAST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += -1;
            mouse->location.y += 1;
            break;
        case SOUTHEAST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += 1;
            mouse->location.y += 1;
            break;
        case SOUTHWEST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += 1;
            mouse->location.y += -1;
            break;
        case NORTHWEST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += -1;
            mouse->location.y += -1;
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

void TurnLeft45(struct Mouse * mouse)
{
    API_turnLeft45();
    mouse->heading = ComputeModulo((int)mouse->heading - 1, NUM_HEADINGS);
}

void TurnRight45(struct Mouse * mouse)
{
    API_turnRight45();
    mouse->heading = ComputeModulo((int)mouse->heading + 1, NUM_HEADINGS);
}

void DebugMouseState(struct Mouse * mouse)
{
    const char* format = "Mouse State: (%d, %d). %s";
    char * heading = GetHeadingStr(mouse->heading);
    int len = snprintf(NULL, 0, format, mouse->location.x, mouse->location.y, heading);
    char msg[len + 1];
    snprintf(msg, len + 1, format, mouse->location.x, mouse->location.y, heading);
    debug_log(msg);
    debug_log("__________________");
}

void SenseWalls(struct Mouse * mouse)
{
    Heading h = mouse->heading;
    if (h == NORTH || h == EAST || h == SOUTH || h == WEST)
    {
        mouse->CheckWallLeft(mouse);
        mouse->CheckWallRight(mouse);
        if (API_wallFront())
        {
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, h);
        }
    }
}

int ComputeModulo(int a, int b)
{
    return ((a % b) + b) % b;
}