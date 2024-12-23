#ifndef MAZE_H
#define MAZE_H
#include "utils.h"


struct Maze {
    char mazeDimension;
    char** maze;
    char** walls;
    void (*SetUp)(struct Maze* maze);
    void (*SetWall)(struct Maze* maze, int x, int y, Heading heading);
    void (*SetCellDistance)(struct Maze* maze, int x, int y, char distance);
    Action (*GetNextMove)(struct Maze* maze, int x, int y, Heading heading);
    char (*IsThereAWall)(struct Maze* maze, int x, int y, Heading heading);
};

struct Maze * CreateMaze(char mazeDimension);
void SetUp(struct Maze* maze);
void SetWall(struct Maze* maze, int x, int y, Heading heading);
void SetCellDistance(struct Maze* maze, int x, int y, char distance);
Action GetNextMove(struct Maze* maze, int x, int y, Heading heading);
char IsThereAWall(struct Maze* maze, int x, int y, Heading heading);
#endif