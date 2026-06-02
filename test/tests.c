#include <assert.h>
#include <stdio.h>

#include "utils.h"
#include "maze.h"

void testMazeWall()
{
    struct Maze * maze = CreateMaze(16);
    maze->SetUpMaze(maze);
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

void testCanMoveDiagonally()
{
    struct Maze * maze = CreateMaze(16);
    maze->SetUpMaze(maze);

    // Interior cell (5,5) - no internal walls set, diagonals should be open
    assert(CanMoveDiagonally(maze, 5, 5, NORTHEAST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHEAST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHWEST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHWEST) == 1);

    // Set NORTH wall at (5,5) - blocks NE and NW
    maze->SetWall(maze, 5, 5, NORTH);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHEAST) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHWEST) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHEAST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHWEST) == 1);

    // Set EAST wall at (5,5) - now NE blocked by both, SE also blocked
    maze->SetWall(maze, 5, 5, EAST);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHEAST) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHEAST) == 0);

    // Corner cells - out of bounds diagonals
    assert(CanMoveDiagonally(maze, 0, 0, NORTHWEST) == 0);
    assert(CanMoveDiagonally(maze, 0, 0, NORTHEAST) == 0);
    assert(CanMoveDiagonally(maze, 15, 15, SOUTHEAST) == 0);

    // Cardinal headings should return 0 (not diagonal)
    assert(CanMoveDiagonally(maze, 5, 5, NORTH) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, EAST) == 0);

    FreeMaze(maze);
}

int main()
{
    testMazeWall();
    testCanMoveDiagonally();
    return 0;
}
