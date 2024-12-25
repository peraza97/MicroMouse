#include <stdio.h>
#include <stdlib.h>
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
    maze->SetUp = &SetUp;
    maze->SetWall = &SetWall;
    maze->GetNextMove = &GetNextMove;
    maze->IsThereAWall = &IsThereAWall;
    return maze;
}

/// @brief SetUp Maze initial values
/// @param maze 
void SetUp(struct Maze* maze)
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
/// @param maze 
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
        FreeQueue(q);
    }

    return;
}

/// @brief Set Wall at location for Given heading
/// @param maze 
/// @param x 
/// @param y 
/// @param heading 
void SetWall(struct Maze* maze, int x, int y, Heading heading)
{
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
    char direction = HeadingsAbbreviation[heading];
    API_setWall(simLoc.x, simLoc.y, direction);
}

/// @brief For given coordinates and direction, is there a wall in front
/// @param maze 
/// @param x 
/// @param y 
/// @param heading 
/// @return 
unsigned char IsThereAWall(struct Maze* maze, int x, int y, Heading heading)
{
    char value = 0;
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
/// @param maze 
/// @param x 
/// @param y 
/// @param distance 
void SetCellDistance(struct Maze* maze, int x, int y, unsigned char distance)
{
    maze->maze[x][y] = distance;
    struct Location simLoc = GetSimulatorCoordinates(x, y);
    char * msg = ConvertNumberToString(distance);
    API_setText(simLoc.x, simLoc.y, msg);
    free(msg);
}

/// @brief Based on current maze state, get best action to get to goal state
/// @param maze 
/// @param x 
/// @param y 
/// @param heading 
/// @return 
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
    if (x >= 1 && !maze->IsThereAWall(maze, x, y, NORTH))
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
    if (y < maze->mazeDimension - 1 && !maze->IsThereAWall(maze, x, y, EAST))
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
    if (x < maze->mazeDimension - 1 && !maze->IsThereAWall(maze, x, y, SOUTH))
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
    if (y >= 1 && !maze->IsThereAWall(maze, x, y, WEST))
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

    char rightTurns = (newHeading - heading + 4) % 4;
    char leftTurns = (heading - newHeading + 4) % 4;
    return leftTurns <= rightTurns ? LEFT : RIGHT;
}

void UpdateMaze(struct Maze * maze, int x, int y, Heading heading)
{
    maze->SetWall(maze, x, y, heading);
    struct Queue * q = QueueInit(255);
    q->QueueEnqueue(q, GetLocationFromCoordinates(x,y));

    while(q->QueueIsEmpty(q) == 0)
    {

        struct Location * loc = q->QueueDequeue(q);
        int tempX = loc->x;
        int tempY = loc->y;
        int dist = maze->maze[tempX][tempY];

        //North
        if (tempX >= 1 && !maze->IsThereAWall(maze, tempX, tempY, NORTH))
        {
            if (maze->maze[tempX-1][tempY] < dist)
            {
                dist = maze->maze[tempX-1][tempY];
            }
            q->QueueEnqueue(q, GetLocationFromCoordinates(tempX-1,tempY));
        }

        //East
        if (tempY < maze->mazeDimension - 1 && !maze->IsThereAWall(maze, tempX, tempY, EAST))
        {
            if (maze->maze[tempX][tempY+1] < dist)
            {
                dist = maze->maze[tempX][tempY+1];
            }
            q->QueueEnqueue(q, GetLocationFromCoordinates(tempX, tempY+1));
        }

        //South
        if (tempX < maze->mazeDimension - 1 && !maze->IsThereAWall(maze, tempX, tempY, SOUTH))
        {
            if (maze->maze[tempX+1][tempY] < dist)
            {
                dist = maze->maze[tempX+1][tempY];
            }
            q->QueueEnqueue(q, GetLocationFromCoordinates(tempX+1, tempY));
        }

        //West
        if (tempY >= 1 && !maze->IsThereAWall(maze, tempX, tempY, WEST))
        {
            if (maze->maze[tempX][tempY-1] < dist)
            {
                dist = maze->maze[tempX][tempY-1];
            }
            q->QueueEnqueue(q, GetLocationFromCoordinates(tempX, tempY-1));
        }
        
        maze->maze[tempX][tempY] = dist;
        free(loc);
    }
    FreeQueue(q);
}
