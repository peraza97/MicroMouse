#include <stdlib.h>
#include <stdio.h>
#include "mouse.h"
#include "API.h"
#include "utils.h"

static const int DX[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
static const int DY[8] = { 0,  1,  1,  1,  0, -1, -1, -1};

struct Mouse * CreateMouse(unsigned char mazeDimension)
{
    struct Mouse *mouse = calloc(1, sizeof(struct Mouse));
    mouse->maze = CreateMaze(mazeDimension);
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
    /* Half-step coordinates: (2 * row + 1, 2 * column + 1). */
    mouse->location.x = mouse->maze->mazeDimension * 2 - 1;
    mouse->location.y = 1;
    mouse->heading = NORTH;
    mouse->maze->SetUpMaze(mouse->maze);
}

void FreeMouse(struct Mouse * mouse)
{
    FreeMaze(mouse->maze);
    free(mouse);
}

Action GetNextAction(struct Mouse * mouse)
{
    return mouse->maze->GetNextMove(mouse->maze, mouse->location.x,
                                    mouse->location.y, mouse->heading);
}

void TakeAction(struct Mouse * mouse, Action action)
{
    switch (action)
    {
        case FORWARD:
            /* One command is one half-step, so a crash never leaves us
               partway through an untracked movement. */
            if (mouse->CanMoveForward(mouse))
                mouse->MoveForward(mouse);
            else
                mouse->maze->UpdateMaze(mouse->maze, mouse->location.x,
                                        mouse->location.y);
            break;
        case LEFT: mouse->TurnLeft(mouse); break;
        case RIGHT: mouse->TurnRight(mouse); break;
        case LEFT45: mouse->TurnLeft45(mouse); break;
        case RIGHT45: mouse->TurnRight45(mouse); break;
        default: break;
    }
}

unsigned char CanMoveForward(struct Mouse * mouse)
{
    (void)mouse;
    return API_wallFront() == 0;
}

void CheckWallLeft(struct Mouse * mouse) { (void)mouse; }
void CheckWallRight(struct Mouse * mouse) { (void)mouse; }

void MoveForward(struct Mouse * mouse)
{
    if (!API_moveForwardHalf())
        return;
    mouse->location.x += DX[mouse->heading];
    mouse->location.y += DY[mouse->heading];
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
    const char* format = "Mouse half-step state: (%d, %d). %s";
    char * heading = GetHeadingStr(mouse->heading);
    int len = snprintf(NULL, 0, format, mouse->location.x, mouse->location.y,
                       heading);
    char msg[len + 1];
    snprintf(msg, len + 1, format, mouse->location.x, mouse->location.y,
             heading);
    debug_log(msg);
}

void SenseWalls(struct Mouse * mouse)
{
    struct HalfStep step;
    DescribeHalfStep(mouse->maze, mouse->location.x, mouse->location.y,
                     mouse->heading, &step);

    if (API_wallFront() && step.isValid && step.isOpen)
    {
        mouse->maze->SetWall(mouse->maze, step.wallX, step.wallY,
                             step.wallHeading);
        RefloodMaze(mouse->maze);
    }
}

int ComputeModulo(int a, int b)
{
    return ((a % b) + b) % b;
}

void SolveMaze(struct Mouse * mouse)
{
    debug_log("Running half-step diagonal solver...");
    Action nextMove;
    do
    {
        mouse->DebugMouseState(mouse);
        mouse->SenseWalls(mouse);
        nextMove = mouse->GetNextAction(mouse);
        mouse->TakeAction(mouse, nextMove);
    } while (nextMove != IDLE);
}
