#ifndef AIM_H
#define AIM_H

#include "alternativa/math/vector3.h"

class Tank;

class Aim
{
public:
	Tank *target;
	Vector3 targetHitPoint;
	Aim(Tank *b, const Vector3 *v);
};

#endif // AIM_H
