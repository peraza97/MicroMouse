#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "maze.h"
#include "utils.h"
#include "API.h"
#include "queue.h"

static const int DX[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
static const int DY[8] = { 0,  1,  1,  1,  0, -1, -1, -1};

static void SetSemiDistance(struct Maze* maze, int x, int y, unsigned char distance);
static unsigned char IsGoal(struct Maze* maze, int x, int y);
static void SetHalfStepWall(struct Maze* maze, struct HalfStep *step,
                            int wallX, int wallY, Heading wallHeading);

struct Maze * CreateMaze(unsigned char mazeDimension)
{
    struct Maze *maze = (struct Maze*)malloc(sizeof (struct Maze));
    maze->mazeDimension = mazeDimension;
    maze->maze = NULL;
    maze->walls = NULL;
    maze->semiDistances = NULL;
    maze->semiDimension = (unsigned char)(mazeDimension * 2 + 1);

    maze->SetCellDistance = &SetCellDistance;
    maze->SetUpMaze = &SetUpMaze;
    maze->SetWall = &SetWall;
    maze->GetNextMove = &GetNextMove;
    maze->IsThereAWall = &IsThereAWall;
    maze->UpdateMaze = &UpdateMaze;
    return maze;
}

/// @brief SetUp Maze initial values
void SetUpMaze(struct Maze* maze)
{
    // Allocate memory
    maze->maze = (unsigned char **)malloc(maze->mazeDimension * sizeof(unsigned char*));
    maze->walls = (unsigned char **)malloc(maze->mazeDimension * sizeof(unsigned char*));
    for (int i = 0; i < maze->mazeDimension; i++) 
    {
        maze->maze[i] = malloc(maze->mazeDimension * sizeof(unsigned char));
        maze->walls[i] = malloc(maze->mazeDimension * sizeof(unsigned char));
    }

    maze->semiDimension = (unsigned char)(maze->mazeDimension * 2 + 1);
    maze->semiDistances = malloc(maze->semiDimension * sizeof(unsigned char *));
    for (int i = 0; i < maze->semiDimension; ++i)
    {
        maze->semiDistances[i] = malloc(maze->semiDimension * sizeof(unsigned char));
    }

    // Set initial cell display values and wall map.
    for (int i = 0; i < maze->mazeDimension; i++) 
    {
        for (int j = 0; j < maze->mazeDimension; j++) 
        {
            maze->SetCellDistance(maze, i, j, 255);
            maze->walls[i][j] = 0;
        }
    }

    // For different size mazes this might need to be changed
    int midPoint = maze->mazeDimension/2;
    maze->SetCellDistance(maze, midPoint, midPoint, 0);
    maze->SetCellDistance(maze, midPoint, midPoint-1, 0);
    maze->SetCellDistance(maze, midPoint-1, midPoint, 0);
    maze->SetCellDistance(maze, midPoint-1, midPoint-1, 0);

    // Set outer walls
    for(int x = 0; x < maze->mazeDimension; ++x)
    {
        maze->SetWall(maze, x, 0, WEST);
        maze->SetWall(maze, 0, x, NORTH);
        maze->SetWall(maze, x, maze->mazeDimension - 1, EAST);
        maze->SetWall(maze, maze->mazeDimension - 1, x, SOUTH);
    }

    // Populate the half-step distance field.
    SetUpInitialDistances(maze);
}

void FreeMaze(struct Maze * maze)
{
    for(int i = 0; i < maze->mazeDimension; i++)
    {
        free(maze->maze[i]);
        free(maze->walls[i]);
    }
    free(maze->maze);
    free(maze->walls);
    for (int i = 0; i < maze->semiDimension; ++i)
    {
        free(maze->semiDistances[i]);
    }
    free(maze->semiDistances);
    free(maze);
}

void SetUpInitialDistances(struct Maze * maze)
{
    struct Queue* q = QueueInit(255);
    int midPoint = maze->mazeDimension/2;

    for (int x = 0; x < maze->semiDimension; ++x)
        for (int y = 0; y < maze->semiDimension; ++y)
            SetSemiDistance(maze, x, y, 255);

    /* Goals are cell centres, not corner posts or edge midpoints. */
    for (int x = midPoint - 1; x <= midPoint; ++x)
    {
        for (int y = midPoint - 1; y <= midPoint; ++y)
        {
            int sx = x * 2 + 1;
            int sy = y * 2 + 1;
            SetSemiDistance(maze, sx, sy, 0);
            q->QueueEnqueue(q, GetLocationFromCoordinates(sx, sy));
        }
    }

    while (q->QueueIsEmpty(q) == 0)
    {
        struct Location * loc = q->QueueDequeue(q);
        unsigned char newDist = maze->semiDistances[loc->x][loc->y] + 1;

        for (int h = 0; h < NUM_HEADINGS; ++h)
        {
            /* Diagonal edge transitions are directional in mms: the wall
               checked at an edge midpoint depends on the exit direction.
               Flood from predecessor states so the stored distance remains a
               true cost-to-go for the forward controller. */
            int px = loc->x - DX[h];
            int py = loc->y - DY[h];
            if (px >= 0 && px < maze->semiDimension &&
                py >= 0 && py < maze->semiDimension)
            {
                struct HalfStep step;
                DescribeHalfStep(maze, px, py, (Heading)h, &step);
                if (!step.isOpen)
                    continue;

                if (maze->semiDistances[px][py] == 255)
                {
                    q->QueueEnqueue(q, GetLocationFromCoordinates(px, py));
                }
                if (maze->semiDistances[px][py] > newDist)
                {
                    SetSemiDistance(maze, px, py, newDist);
                }
            }
        }

        free(loc);
    }
    FreeQueue(q);

    /* Keep the familiar cell-centre visualisation in mms. */
    for (int x = 0; x < maze->mazeDimension; ++x)
        for (int y = 0; y < maze->mazeDimension; ++y)
            maze->SetCellDistance(maze, x, y,
                                  maze->semiDistances[x * 2 + 1][y * 2 + 1]);
}

void RefloodMaze(struct Maze* maze)
{
    SetUpInitialDistances(maze);
}

/// @brief Set Wall at location for Given heading
void SetWall(struct Maze* maze, int x, int y, Heading heading)
{
    switch (heading)
    {
        case NORTH:
            SetWallHelper(maze, x, y, NORTH);
            SetWallHelper(maze, x-1, y, SOUTH);
            break;
        case EAST:
            SetWallHelper(maze, x, y, EAST);
            SetWallHelper(maze, x, y+1, WEST);
            break;
        case SOUTH:
            SetWallHelper(maze, x, y, SOUTH);
            SetWallHelper(maze, x+1, y, NORTH);
            break;
        case WEST:
            SetWallHelper(maze, x, y, WEST);
            SetWallHelper(maze, x, y-1, EAST);
            break;
        default:
            break;
    }
}

void SetWallHelper(struct Maze* maze, int x, int y, Heading heading)
{
    if (x < 0 || y < 0 || x >= maze->mazeDimension || y >= maze->mazeDimension)
    {
        return;
    }

    switch (heading)
    {
        case NORTH:
            maze->walls[x][y] |= 1 << 3;
            break;
        case EAST:
            maze->walls[x][y] |= 1 << 2;
            break;
        case SOUTH:
            maze->walls[x][y] |= 1 << 1;
            break;
        case WEST:
            maze->walls[x][y] |= 1 << 0;
            break;
        default:
            break;
    }

    // API
    struct Location simLoc = GetSimulatorCoordinates(x, y);
    API_setWall(simLoc.x, simLoc.y, GetHeadingAbbreviation(heading));
}

/// @brief For given coordinates and direction, is there a wall in front
unsigned char IsThereAWall(struct Maze* maze, int x, int y, Heading heading)
{
    unsigned char value = 0;
    switch (heading)
    {
        case NORTH:
            value = 1 << 3;
            break;
        case EAST:
            value = 1 << 2;
            break;
        case SOUTH:
            value = 1 << 1;
            break;
        case WEST:
            value = 1 << 0;
            break;
        default:
            break;
    }

    return maze->walls[x][y] & value;
}

unsigned char CanMoveDiagonally(struct Maze* maze, int x, int y, Heading heading)
{
    int dx = 0, dy = 0;
    Heading wall1, wall2;

    switch (heading)
    {
        case NORTHEAST:
            dx = -1; dy = 1;
            wall1 = NORTH; wall2 = EAST;
            break;
        case SOUTHEAST:
            dx = 1; dy = 1;
            wall1 = SOUTH; wall2 = EAST;
            break;
        case SOUTHWEST:
            dx = 1; dy = -1;
            wall1 = SOUTH; wall2 = WEST;
            break;
        case NORTHWEST:
            dx = -1; dy = -1;
            wall1 = NORTH; wall2 = WEST;
            break;
        default:
            return 0;
    }

    int newX = x + dx;
    int newY = y + dy;
    if (newX < 0 || newX >= maze->mazeDimension || newY < 0 || newY >= maze->mazeDimension)
    {
        return 0;
    }

    if (maze->IsThereAWall(maze, x, y, wall1) || maze->IsThereAWall(maze, x, y, wall2))
    {
        return 0;
    }

    // Check walls from the two cells adjacent to the shared corner
    int adjX1 = x + DX[wall1];
    int adjY1 = y + DY[wall1];
    if (adjX1 >= 0 && adjX1 < maze->mazeDimension && adjY1 >= 0 && adjY1 < maze->mazeDimension)
    {
        if (maze->IsThereAWall(maze, adjX1, adjY1, wall2))
            return 0;
    }

    int adjX2 = x + DX[wall2];
    int adjY2 = y + DY[wall2];
    if (adjX2 >= 0 && adjX2 < maze->mazeDimension && adjY2 >= 0 && adjY2 < maze->mazeDimension)
    {
        if (maze->IsThereAWall(maze, adjX2, adjY2, wall1))
            return 0;
    }

    return 1;
}

/*
 * Single source of truth for the doubled-grid geometry. mms permits cell
 * centres and edge midpoints, never corner posts. Every structurally valid
 * half-step is associated with one cardinal wall, which is used both to plan
 * around known walls and to record a wall reported by the simulator.
 */
void DescribeHalfStep(struct Maze* maze, int x, int y, Heading heading,
                      struct HalfStep *step)
{
    int newX = x + DX[heading];
    int newY = y + DY[heading];

    *step = (struct HalfStep){0};
    step->nextX = newX;
    step->nextY = newY;

    if (newX < 0 || newX >= maze->semiDimension ||
        newY < 0 || newY >= maze->semiDimension ||
        (newX % 2 == 0 && newY % 2 == 0))
        return;

    /* Cell centre: only cardinal moves can leave it. */
    if (x % 2 == 1 && y % 2 == 1)
    {
        if (heading % 2 == 0)
            SetHalfStepWall(maze, step, x / 2, y / 2, heading);
        return;
    }

    /* Horizontal cell boundary (between rows r - 1 and r). */
    if (x % 2 == 0 && y % 2 == 1)
    {
        int r = x / 2;
        int c = y / 2;
        if ((heading == NORTH || heading == SOUTH) &&
            r > 0 && r < maze->mazeDimension)
            SetHalfStepWall(maze, step, r, c, NORTH);
        else if ((heading == NORTHEAST || heading == NORTHWEST) && r > 0)
            SetHalfStepWall(maze, step, r - 1, c,
                            heading == NORTHEAST ? EAST : WEST);
        else if ((heading == SOUTHEAST || heading == SOUTHWEST) &&
                 r < maze->mazeDimension)
            SetHalfStepWall(maze, step, r, c,
                            heading == SOUTHEAST ? EAST : WEST);
        return;
    }

    /* Vertical cell boundary (between columns c - 1 and c). */
    if (x % 2 == 1 && y % 2 == 0)
    {
        int r = x / 2;
        int c = y / 2;
        if ((heading == EAST || heading == WEST) &&
            c > 0 && c < maze->mazeDimension)
            SetHalfStepWall(maze, step, r, c, WEST);
        else if ((heading == NORTHEAST || heading == SOUTHEAST) &&
                 c < maze->mazeDimension)
            SetHalfStepWall(maze, step, r, c,
                            heading == NORTHEAST ? NORTH : SOUTH);
        else if ((heading == NORTHWEST || heading == SOUTHWEST) && c > 0)
            SetHalfStepWall(maze, step, r, c - 1,
                            heading == NORTHWEST ? NORTH : SOUTH);
    }
}

static void SetHalfStepWall(struct Maze* maze, struct HalfStep *step,
                            int wallX, int wallY, Heading wallHeading)
{
    step->isValid = 1;
    step->wallX = wallX;
    step->wallY = wallY;
    step->wallHeading = wallHeading;
    step->isOpen = maze->IsThereAWall(maze, wallX, wallY, wallHeading) == 0;
}

static void SetSemiDistance(struct Maze* maze, int x, int y, unsigned char distance)
{
    maze->semiDistances[x][y] = distance;
}

static unsigned char IsGoal(struct Maze* maze, int x, int y)
{
    int firstGoal = maze->mazeDimension - 1;
    int secondGoal = maze->mazeDimension + 1;
    return (x == firstGoal || x == secondGoal) &&
           (y == firstGoal || y == secondGoal);
}

/// @brief For given coordinates, state it is X from goal state
void SetCellDistance(struct Maze* maze, int x, int y, unsigned char distance)
{
    maze->maze[x][y] = distance;

    //API Helper
    struct Location simLoc = GetSimulatorCoordinates(x, y);
    API_setNumText(simLoc.x, simLoc.y, distance);
}

/// @brief Based on current maze state, get best action to get to goal state
Action GetNextMove(struct Maze* maze, int x, int y, Heading heading)
{
    if (IsGoal(maze, x, y))
    {
        return IDLE;
    }

    Heading newHeading = heading;
    int dist = 255;
    int turnCost = NUM_HEADINGS;

    for (int h = 0; h < NUM_HEADINGS; ++h)
    {
        struct HalfStep step;
        DescribeHalfStep(maze, x, y, (Heading)h, &step);
        if (step.isOpen)
        {
            int nx = step.nextX;
            int ny = step.nextY;
            int candidateTurnCost = ((h - heading + NUM_HEADINGS) % NUM_HEADINGS);
            int leftTurnCost = ((heading - h + NUM_HEADINGS) % NUM_HEADINGS);
            if (leftTurnCost < candidateTurnCost)
                candidateTurnCost = leftTurnCost;

            if (maze->semiDistances[nx][ny] < dist ||
                (maze->semiDistances[nx][ny] == dist &&
                 candidateTurnCost < turnCost))
            {
                newHeading = (Heading)h;
                dist = maze->semiDistances[nx][ny];
                turnCost = candidateTurnCost;
            }
        }
    }

    if (heading == newHeading)
    {
        return FORWARD;
    }

    char rightTurns = (newHeading - heading + NUM_HEADINGS) % NUM_HEADINGS;
    char leftTurns = (heading - newHeading + NUM_HEADINGS) % NUM_HEADINGS;

    if (rightTurns == 1) return RIGHT45;
    if (leftTurns == 1) return LEFT45;
    return leftTurns <= rightTurns ? LEFT : RIGHT;
}

/// @brief Last action hit a newly discovered wall. use floodfill to update maze with new distances
void UpdateMaze(struct Maze * maze, int x, int y)
{
    (void)x;
    (void)y;
    RefloodMaze(maze);
}
