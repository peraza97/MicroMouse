#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "maze.h"
#include "utils.h"
#include "API.h"
#include "queue.h"

static const int DX[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
static const int DY[8] = { 0,  1,  1,  1,  0, -1, -1, -1};

static unsigned char CanMoveInDirection(struct Maze* maze, int x, int y, Heading heading);

struct Maze * CreateMaze(unsigned char mazeDimension)
{
    struct Maze *maze = (struct Maze*)malloc(sizeof (struct Maze));
    maze->mazeDimension = mazeDimension;
    maze->maze = (unsigned char **)malloc(maze->mazeDimension * sizeof(unsigned char *));

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
        maze->maze[i] = (char *)malloc(maze->mazeDimension * sizeof(unsigned char));
        maze->walls[i] = (char *)malloc(maze->mazeDimension * sizeof(unsigned char));
    }

    // Set initial distances
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

    //try and set up initial distances
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
    free(maze);
}

void SetUpInitialDistances(struct Maze * maze)
{
    struct Queue* q = QueueInit(255);
    int midPoint = maze->mazeDimension/2;

    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint, midPoint));
    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint, midPoint-1));
    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint-1, midPoint));
    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint-1, midPoint-1));

    while (q->QueueIsEmpty(q) == 0)
    {
        struct Location * loc = q->QueueDequeue(q);
        unsigned char newDist = maze->maze[loc->x][loc->y] + 1;

        for (int h = 0; h < NUM_HEADINGS; ++h)
        {
            if (CanMoveInDirection(maze, loc->x, loc->y, (Heading)h))
            {
                int nx = loc->x + DX[h];
                int ny = loc->y + DY[h];
                if (maze->maze[nx][ny] == 255)
                {
                    q->QueueEnqueue(q, GetLocationFromCoordinates(nx, ny));
                }
                if (maze->maze[nx][ny] > newDist)
                {
                    maze->SetCellDistance(maze, nx, ny, newDist);
                }
            }
        }

        free(loc);
    }
    free(q);
}

void RefloodMaze(struct Maze* maze)
{
    int midPoint = maze->mazeDimension / 2;
    for (int i = 0; i < maze->mazeDimension; i++)
        for (int j = 0; j < maze->mazeDimension; j++)
            maze->SetCellDistance(maze, i, j, 255);

    maze->SetCellDistance(maze, midPoint, midPoint, 0);
    maze->SetCellDistance(maze, midPoint, midPoint - 1, 0);
    maze->SetCellDistance(maze, midPoint - 1, midPoint, 0);
    maze->SetCellDistance(maze, midPoint - 1, midPoint - 1, 0);

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

static unsigned char CanMoveInDirection(struct Maze* maze, int x, int y, Heading heading)
{
    int newX = x + DX[heading];
    int newY = y + DY[heading];

    if (newX < 0 || newX >= maze->mazeDimension || newY < 0 || newY >= maze->mazeDimension)
    {
        return 0;
    }

    switch (heading)
    {
        case NORTH:
        case EAST:
        case SOUTH:
        case WEST:
            return maze->IsThereAWall(maze, x, y, heading) == 0;
        default:
            return CanMoveDiagonally(maze, x, y, heading);
    }
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
    if (maze->maze[x][y] == 0)
    {
        return IDLE;
    }

    Heading newHeading = heading;
    int dist = 255;

    for (int h = 0; h < NUM_HEADINGS; ++h)
    {
        if (CanMoveInDirection(maze, x, y, (Heading)h))
        {
            int nx = x + DX[h];
            int ny = y + DY[h];
            if (maze->maze[nx][ny] < dist)
            {
                newHeading = (Heading)h;
                dist = maze->maze[nx][ny];
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
    struct Queue * q = QueueInit(255);
    q->QueueEnqueue(q, GetLocationFromCoordinates(x, y));

    while(q->QueueIsEmpty(q) == 0)
    {
        struct Location * loc = q->QueueDequeue(q);

        struct Location * accessibleNeighbors[8];
        int neighborIndex = 0;

        for (int h = 0; h < NUM_HEADINGS; ++h)
        {
            if (CanMoveInDirection(maze, loc->x, loc->y, (Heading)h))
            {
                int nx = loc->x + DX[h];
                int ny = loc->y + DY[h];
                accessibleNeighbors[neighborIndex++] = GetLocationFromCoordinates(nx, ny);
            }
        }

        if (neighborIndex > 0)
        {
            unsigned char min = 255;
            for(int i = 0; i < neighborIndex; ++i)
            {
                if (maze->maze[accessibleNeighbors[i]->x][accessibleNeighbors[i]->y] <= min)
                {
                    min = maze->maze[accessibleNeighbors[i]->x][accessibleNeighbors[i]->y];
                }
            }

            if (maze->maze[loc->x][loc->y] <= min)
            {
                maze->SetCellDistance(maze, loc->x, loc->y, min+1);
                for(int i = 0; i < neighborIndex; ++i)
                {
                    q->QueueEnqueue(q, accessibleNeighbors[i]);
                }
            }
            else
            {
                for(int i = 0; i < neighborIndex; ++i)
                {
                    free(accessibleNeighbors[i]);
                }
            }
        }
        free(loc);
    }
    FreeQueue(q);
}
