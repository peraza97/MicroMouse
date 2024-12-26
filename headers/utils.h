#ifndef UTILS_H
#define UTILS_H

extern char* HeadingsAbbreviation;
typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

struct Location
{
    int x;
    int y;
};

struct Location* GetLocationFromCoordinates(int x, int y);

// Print helpers
void PrintNextAction(Action nextMove);
void PrintHeading(Heading heading);
void PrintLocation(int x, int y);

// Simulator Helper
struct Location GetSimulatorCoordinates(int x, int y);
struct Location GetSimulatorCoordinatesFromLocation(struct Location loc);
#endif