#ifndef SOLVER_H
#define SOLVER_H

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

struct Location
{
    int x;
    int y;
};

Action solver();
Action leftWallFollower();
Action floodFill();

#endif