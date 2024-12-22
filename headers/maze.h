#ifndef MAZE_H
#define MAZE_H

struct Maze {
    char mazeDimension;
    char** maze;
    char** walls;
    void (*SetUp)(struct Maze* maze);
    void (*SetWall)(struct Maze* maze, int x, int y, char direction);
    void (*SetCellDistance)(struct Maze* maze, int x, int y, char distance);
};

struct Maze * CreateMaze(char mazeDimension);
void SetUp(struct Maze* maze);
void SetWall(struct Maze* maze, int x, int y, char direction);
void SetCellDistance(struct Maze* maze, int x, int y, char distance);
#endif