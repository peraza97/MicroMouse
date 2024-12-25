#include <assert.h>
#include <stdio.h>

#include "utils.h"
#include "maze.h"

void testMazeWall()
{
    struct Maze * maze = CreateMaze(16);
    maze->SetUp(maze);
    assert(maze->IsThereAWall(maze,1,1,NORTH) == 0);
    assert(maze->IsThereAWall(maze,1,1,EAST) == 0);
    assert(maze->IsThereAWall(maze,1,1,SOUTH) == 0);
    assert(maze->IsThereAWall(maze,1,1,WEST) == 0);
    
    // Settimg walls
    maze->SetWall(maze, 1, 1, NORTH);
    assert(maze->IsThereAWall(maze,1,1,NORTH) == 8);
    maze->SetWall(maze, 1, 1, EAST);
    assert(maze->IsThereAWall(maze,1,1,EAST) == 4);
    maze->SetWall(maze, 1, 1, SOUTH);
    assert(maze->IsThereAWall(maze,1,1,SOUTH) == 2);
    maze->SetWall(maze, 1, 1, WEST);
    assert(maze->IsThereAWall(maze,1,1,WEST) == 1);
    
    FreeMaze(maze);
}

int main()
{
    testMazeWall();
    return 0;
}
