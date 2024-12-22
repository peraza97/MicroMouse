#ifndef MOUSE_H
#define MOUSE_H

struct FloodFillSolver {
    struct Maze * Maze;
    void (*Setup)();
};

struct FloodFillSolver * CreateFloodFillSolver(int mazeDimension);
void FloodFillSetup();

#endif