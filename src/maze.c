#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

struct Maze * CreateMaze(char mazeDimension)
{
    struct Maze *maze = malloc(sizeof (struct Maze));
    maze->mazeDimension = mazeDimension;
    maze->maze = (char **)malloc(maze->mazeDimension * sizeof(char *));

    maze->SetCellDistance = &SetCellDistance;
    maze->SetUp = &SetUp;
    maze->SetWall = &SetWall;

    maze->SetUp(maze);

    return maze;
}

void SetUp(struct Maze* maze)
{
    maze->maze = (char **)malloc(maze->mazeDimension * sizeof(char *));
    for (int i = 0; i < maze->mazeDimension; i++) 
    {
        maze->maze[i] = (char *)malloc(maze->mazeDimension * sizeof(char));
    }

    for (int i = 0; i < maze->mazeDimension; i++) 
    {
        for (int j = 0; j < maze->mazeDimension; j++) 
        {
            maze->SetCellDistance(maze, i, j, 255);
        }
    }
    int midPoint = maze->mazeDimension/2;
    maze->SetCellDistance(maze, midPoint, midPoint, 0);
    maze->SetCellDistance(maze, midPoint, midPoint-1, 0);
    maze->SetCellDistance(maze, midPoint-1, midPoint, 0);
    maze->SetCellDistance(maze, midPoint-1, midPoint-1, 0);
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
}

void SetCellDistance(struct Maze* maze, int x, int y, char distance)
{
    maze->maze[x][y] = distance;
}