#ifndef SSHIP
#define SSHIP

#include "vector.h"


void ShipDraw(vector pos, vector forward);
void moveShip(vector* shipPos, vector forward, double speed);

#endif