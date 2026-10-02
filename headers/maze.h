#ifndef MAZE_H
#define MAZE_H
#include "utils.h"

/* Complete description of one mms moveForwardHalf command. */
struct HalfStep {
    unsigned char isValid;
    unsigned char isOpen;
    int nextX;
    int nextY;
    int wallX;
    int wallY;
    Heading wallHeading;
};

struct Maze {
    unsigned char mazeDimension;
    unsigned char** maze;
    unsigned char** walls;
    unsigned char** semiDistances;
    unsigned char semiDimension;
    void (*SetUpMaze)(struct Maze* maze);
    void (*SetWall)(struct Maze* maze, int x, int y, Heading heading);
    void (*SetCellDistance)(struct Maze* maze, int x, int y, unsigned char distance);
    Action (*GetNextMove)(struct Maze* maze, int x, int y, Heading heading);
    unsigned char (*IsThereAWall)(struct Maze* maze, int x, int y, Heading heading);
    void (*UpdateMaze)(struct Maze* maze, int x, int y);
};

struct Maze * CreateMaze(unsigned char mazeDimension);
void FreeMaze(struct Maze * maze);

void SetUpMaze(struct Maze* maze);
void SetUpInitialDistances(struct Maze * maze);
void SetWall(struct Maze* maze, int x, int y, Heading heading);
void SetWallHelper(struct Maze* maze, int x, int y, Heading heading);
unsigned char IsThereAWall(struct Maze* maze, int x, int y, Heading heading);
unsigned char CanMoveDiagonally(struct Maze* maze, int x, int y, Heading heading);
void DescribeHalfStep(struct Maze* maze, int x, int y, Heading heading,
                      struct HalfStep *step);
void SetCellDistance(struct Maze* maze, int x, int y, unsigned char distance);
Action GetNextMove(struct Maze* maze, int x, int y, Heading heading);
void UpdateMaze(struct Maze* maze, int x, int y);
void RefloodMaze(struct Maze* maze);
#endif
