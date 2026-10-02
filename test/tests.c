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

void testHalfStepFloodfillUsesDiagonal()
{
    struct Maze * maze = CreateMaze(16);
    maze->SetUpMaze(maze);

    /* Start cell centre, then its north edge midpoint.  The first move must
       be cardinal; mms blocks a 45-degree move directly out of a centre. */
    assert(maze->GetNextMove(maze, 31, 1, NORTH) == FORWARD);
    int x = 31, y = 1;
    Heading heading = NORTH;
    unsigned char sawDiagonalTurn = 0;
    const int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    for (int moves = 0; moves < 128; ++moves)
    {
        Action action = maze->GetNextMove(maze, x, y, heading);
        if (action == IDLE)
            break;
        if (action == LEFT45 || action == RIGHT45)
            sawDiagonalTurn = 1;
        if (action == FORWARD)
        {
            x += dx[heading];
            y += dy[heading];
        }
        else if (action == LEFT)
            heading = (Heading)((heading + 6) % 8);
        else if (action == RIGHT)
            heading = (Heading)((heading + 2) % 8);
        else if (action == LEFT45)
            heading = (Heading)((heading + 7) % 8);
        else if (action == RIGHT45)
            heading = (Heading)((heading + 1) % 8);
    }

    assert(sawDiagonalTurn == 1);
    assert((x == 15 || x == 17) && (y == 15 || y == 17));

    FreeMaze(maze);
}

void testHalfStepDescription()
{
    struct Maze * maze = CreateMaze(16);
    maze->SetUpMaze(maze);
    struct HalfStep step;

    DescribeHalfStep(maze, 31, 1, NORTH, &step);
    assert(step.isValid == 1 && step.isOpen == 1);
    assert(step.nextX == 30 && step.nextY == 1);
    assert(step.wallX == 15 && step.wallY == 0 && step.wallHeading == NORTH);

    /* A diagonal cannot start at a cell centre. */
    DescribeHalfStep(maze, 31, 1, NORTHEAST, &step);
    assert(step.isValid == 0);

    /* At the north edge midpoint, NE is legal and checks this east wall. */
    DescribeHalfStep(maze, 30, 1, NORTHEAST, &step);
    assert(step.isValid == 1 && step.isOpen == 1);
    assert(step.nextX == 29 && step.nextY == 2);
    assert(step.wallX == 14 && step.wallY == 0 && step.wallHeading == EAST);

    maze->SetWall(maze, 14, 0, EAST);
    DescribeHalfStep(maze, 30, 1, NORTHEAST, &step);
    assert(step.isValid == 1 && step.isOpen == 0);

    FreeMaze(maze);
}

int main()
{
    testMazeWall();
    testCanMoveDiagonally();
    testHalfStepFloodfillUsesDiagonal();
    testHalfStepDescription();
    return 0;
}
