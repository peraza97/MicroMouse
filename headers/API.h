#pragma once

int API_mazeWidth();
int API_mazeHeight();

int API_wallFront();
int API_wallRight();
int API_wallLeft();

int API_moveForward();  // Returns 0 if crash, else returns 1
void API_turnRight();
void API_turnLeft();

void API_turnRight45();
void API_turnLeft45();

void API_setWall(int x, int y, char direction);
void API_clearWall(int x, int y, char direction);

void API_setColor(int x, int y, char color);
void API_clearColor(int x, int y);
void API_clearAllColor();

void API_setText(int x, int y, char* str);
void API_setNumText(int x, int y, unsigned char value);
void API_clearText(int x, int y);
void API_clearAllText();

int API_wasReset();
void API_ackReset();

void debug_log(char* text);
void debug_Numlog(unsigned char value);