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

void SetWall(struct Maze* maze, int x, int y, char direction)
{
    switch (direction)
    {
        case'n':
            maze->walls[x][y] |= 2 << 3;
            break;
        case'e':
            maze->walls[x][y] |= 2 << 2;
            break;
        case's':
            maze->walls[x][y] |= 2 << 1;
            break;
        case'w':
            maze->walls[x][y] |= 2 << 0;
            break;
        default:
            break;
    }

    // API
    struct Location simLoc = GetSimulatorCoordinates(x, y);
    API_setWall(simLoc.x, simLoc.y, direction);
}

void SetCellDistance(struct Maze* maze, int x, int y, char distance)
{
    maze->maze[x][y] = distance;
}