#ifndef TANKSTATE_H
#define TANKSTATE_H

#include "alternativa/math/vector3.h"

class TankState
{
public:
	int health;
	Vector3 position;
	Vector3 orientation;
	int turretAngle;
	int control;
};

#endif // TANKSTATE_H
