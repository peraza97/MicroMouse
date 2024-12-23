#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "utils.h"
#include "API.h"

struct Maze * CreateMaze(char mazeDimension)
{
    struct Maze *maze = malloc(sizeof (struct Maze));
    maze->mazeDimension = mazeDimension;
    maze->maze = (char **)malloc(maze->mazeDimension * sizeof(char *));

    maze->SetCellDistance = &SetCellDistance;
    maze->SetUp = &SetUp;
    maze->SetWall = &SetWall;
    maze->GetNextMove = &GetNextMove;
    maze->IsThereAWall = &IsThereAWall;
    return maze;
}

void SetUp(struct Maze* maze)
{
    maze->maze = (char **)malloc(maze->mazeDimension * sizeof(char *));
    maze->walls = (char **)malloc(maze->mazeDimension * sizeof(char *));
    for (int i = 0; i < maze->mazeDimension; i++) 
    {
        maze->maze[i] = (char *)malloc(maze->mazeDimension * sizeof(char));
        maze->walls[i] = (char *)malloc(maze->mazeDimension * sizeof(char));
    }

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

    for(int x = 0; x < maze->mazeDimension; ++x)
    {
        maze->SetWall(maze, x, 0, HeadingsAbbreviation[WEST]);
        maze->SetWall(maze, 0, x, HeadingsAbbreviation[NORTH]);
        maze->SetWall(maze, x, maze->mazeDimension - 1, HeadingsAbbreviation[EAST]);
        maze->SetWall(maze, maze->mazeDimension - 1, x, HeadingsAbbreviation[SOUTH]);
    }
}

void SetWall(struct Maze* maze, int x, int y, Heading heading)
{
    switch (heading)
    {
        case NORTH:
            maze->walls[x][y] |= 2 << 3;
            break;
        case EAST:
            maze->walls[x][y] |= 2 << 2;
            break;
        case SOUTH:
            maze->walls[x][y] |= 2 << 1;
            break;
        case WEST:
            maze->walls[x][y] |= 2 << 0;
            break;
        default:
            break;
    }

    // API
    struct Location simLoc = GetSimulatorCoordinates(x, y);
    char direction = HeadingsAbbreviation[heading];
    API_setWall(simLoc.x, simLoc.y, direction);
}

void SetCellDistance(struct Maze* maze, int x, int y, char distance)
{
    maze->maze[x][y] = distance;
}

char IsThereAWall(struct Maze* maze, int x, int y, Heading heading)
{
    char value = 0;
    switch (heading)
    {
        case NORTH:
            value = 2 << 3;
            break;
        case EAST:
            value = 2 << 2;
            break;
        case SOUTH:
            value = 2 << 1;
            break;
        case WEST:
            value = 2 << 0;
            break;
        default:
            break;
    }

    return maze->walls[x][y] & value;
}

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
    if (x > 1 && !maze->IsThereAWall(maze, x-1, y, heading))
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
    if (y < maze->mazeDimension - 1 && !maze->IsThereAWall(maze, x, y+1, heading))
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
    if (x < maze->mazeDimension - 1 && !maze->IsThereAWall(maze, x+1, y, heading))
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
    if (y > 1)
    {
        if (maze->maze[x][y-1] < dist && !maze->IsThereAWall(maze, x, y-1, heading))
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