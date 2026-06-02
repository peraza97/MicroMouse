#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "maze.h"
#include "utils.h"
#include "API.h"
#include "queue.h"

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

/// @brief Set up initial distances using Manhatten distances
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
        int tempX = loc->x;
        int tempY = loc->y;

        //North
        if (loc->x >= 1)
        {
            tempX = loc->x-1;
            tempY = loc->y;
            if (maze->maze[tempX][tempY] == 255)
            {
             q->QueueEnqueue(q, GetLocationFromCoordinates(tempX, tempY));
            }
            if (maze->maze[tempX][tempY] > newDist)
            {
                maze->SetCellDistance(maze, tempX, tempY, newDist);
            }
        }

        //East
        if (loc->y < maze->mazeDimension - 1)
        {
            tempX = loc->x;
            tempY = loc->y+1;
            if (maze->maze[tempX][tempY] == 255)
            {
             q->QueueEnqueue(q, GetLocationFromCoordinates(tempX, tempY));
            }
            if (maze->maze[tempX][tempY] > newDist)
            {
                maze->SetCellDistance(maze, tempX, tempY, newDist);
            }
        }

        //South
        if (loc->x < maze->mazeDimension - 1)
        {
            tempX = loc->x+1;
            tempY = loc->y;
            if (maze->maze[tempX][tempY] == 255)
            {
             q->QueueEnqueue(q, GetLocationFromCoordinates(tempX, tempY));
            }
            if (maze->maze[tempX][tempY] > newDist)
            {
                maze->SetCellDistance(maze, tempX, tempY, newDist);
            }
        }

        //West
        if (loc->y >= 1)
        {
            tempX = loc->x;
            tempY = loc->y-1;
            if (maze->maze[tempX][tempY] == 255)
            {
             q->QueueEnqueue(q, GetLocationFromCoordinates(tempX, tempY));
            }
            if (maze->maze[tempX][tempY] > newDist)
            {
                maze->SetCellDistance(maze, tempX, tempY, newDist);
            }
        }

        free(loc);
    }
    free(q);
    return;
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

    int newX = x;
    int newY = y;
    Heading newHeading;
    int dist = 255;

    //North
    if (x >= 1 && maze->IsThereAWall(maze, x, y, NORTH) == 0)
    {
        if (maze->maze[x-1][y] < dist)
        {
            newX = x-1;
            newY = y;
            newHeading = NORTH;
            dist = maze->maze[x-1][y];
        }
    }

    //East
    if (y < maze->mazeDimension - 1 && maze->IsThereAWall(maze, x, y, EAST) == 0)
    {
        if (maze->maze[x][y+1] < dist)
        {
            newX = x;
            newY = y+1;
            newHeading = EAST;
            dist = maze->maze[x][y+1];
        }
    }

    //South
    if (x < maze->mazeDimension - 1 && maze->IsThereAWall(maze, x, y, SOUTH) == 0)
    {
        if (maze->maze[x+1][y] < dist)
        {
            newX = x+1;
            newY = y;
            newHeading = SOUTH;
            dist = maze->maze[x+1][y];
        }
    }

    //West
    if (y >= 1 && maze->IsThereAWall(maze, x, y, WEST) == 0)
    {
        if (maze->maze[x][y-1] < dist)
        {
            newX = x;
            newY = y-1;
            newHeading = WEST;
            dist = maze->maze[x][y-1];
        }
    }

    if (heading == newHeading)
    {
        return FORWARD;
    }
    char rightTurns = (newHeading - heading + NUM_HEADINGS) % NUM_HEADINGS;
    char leftTurns = (heading - newHeading + NUM_HEADINGS) % NUM_HEADINGS;
    return leftTurns <= rightTurns ? LEFT : RIGHT;
}

/// @brief Last action hit a newly discovered wall. use floodfill to update maze with new distances
void UpdateMaze(struct Maze * maze, int x, int y)
{
    struct Queue * q = QueueInit(255);
    q->QueueEnqueue(q, GetLocationFromCoordinates(x,y));

    while(q->QueueIsEmpty(q) == 0)
    {
        struct Location * loc = q->QueueDequeue(q);

        // set up accessible neighbors
        struct Location * accessibleNeighbors[5];
        int neighborIndex = 0;
        unsigned char found = 0;

        //North
        if (loc->x >= 1 && maze->IsThereAWall(maze, loc->x, loc->y, NORTH) == 0)
        {
            found = 1;
            accessibleNeighbors[neighborIndex++] = GetLocationFromCoordinates(loc->x-1,loc->y);
        }

        //East
        if (loc->y < maze->mazeDimension - 1 && maze->IsThereAWall(maze, loc->x, loc->y, EAST) == 0)
        {
            found = 1;
            accessibleNeighbors[neighborIndex++] = GetLocationFromCoordinates(loc->x, loc->y+1);
        }

        //South
        if (loc->x < maze->mazeDimension - 1 && maze->IsThereAWall(maze, loc->x, loc->y, SOUTH) == 0)
        {
            found = 1;
            accessibleNeighbors[neighborIndex++] = GetLocationFromCoordinates(loc->x+1, loc->y);
        }

        //West
        if (loc->y >= 1 && maze->IsThereAWall(maze, loc->x, loc->y, WEST) == 0)
        {
            found = 1;
            accessibleNeighbors[neighborIndex++] = GetLocationFromCoordinates(loc->x, loc->y-1);
        }

        // there is at least 1 accessible neighbor, check if it needs an update
        if (found > 0)
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
