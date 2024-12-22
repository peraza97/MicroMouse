#ifndef UTILS_H
#define UTILS_H

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

struct Location
{
    int x;
    int y;
};

extern char* HeadingsAbbreviation;
char* PrintNextAction(Action nextMove);
void PrintHeading(Heading heading);
void PrintLocationFromLocation(struct Location);
void PrintLocation(int x, int y);
struct Location GetSimulatorCoordinatesFromLocation(struct Location loc);
struct Location GetSimulatorCoordinates(int x, int y);
#endif