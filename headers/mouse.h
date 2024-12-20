#ifndef MOUSE_H
#define MOUSE_H
#include "solver.h"


struct Mouse {
    Heading heading;
    struct Location location;
    void (*TakeAction)(struct Mouse * mouse, Action action);
    void (*PrintLocation)(struct Mouse * mouse);
    void (*PrintHeading)(struct Mouse * mouse);
};

struct Mouse * CreateMouse();
void FreeMouse(struct Mouse * mouse);

void TakeAction(struct Mouse * mouse, Action action);
void PrintLocation(struct Mouse * mouse);
void PrintHeading(struct Mouse * mouse);
int ComputeModulo(int a, int b);

#endif