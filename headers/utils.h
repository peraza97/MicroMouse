#ifndef UTILS_H
#define UTILS_H


typedef enum Action {LEFT, LEFT45, FORWARD, RIGHT, RIGHT45, IDLE} Action;
typedef enum Heading {NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST} Heading;
#define NUM_HEADINGS 8

struct Location
{
    int x;
    int y;
};

char GetHeadingAbbreviation(Heading heading);
struct Location* GetLocationFromCoordinates(int x, int y);

char * GetHeadingStr(Heading heading);

// Print helpers
void PrintNextAction(Action nextMove);
void PrintLocation(int x, int y);

// Simulator Helper
struct Location GetSimulatorCoordinates(int x, int y);
struct Location GetSimulatorCoordinatesFromLocation(struct Location loc);
#endif