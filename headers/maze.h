#ifndef MAZE_H
#define MAZE_H
#include "utils.h"


struct Maze {
    unsigned char mazeDimension;
    unsigned char** maze;
    unsigned char** walls;
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
void SetCellDistance(struct Maze* maze, int x, int y, unsigned char distance);
Action GetNextMove(struct Maze* maze, int x, int y, Heading heading);
void UpdateMaze(struct Maze* maze, int x, int y);
#endif
