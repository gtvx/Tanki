#ifndef HITINFO_H
#define HITINFO_H

#include "alternativa/math/vector3.h"

class Body;

class HitInfo
{
public:
	double distance;
	Body *body;
	Vector3 position;
	Vector3 direction;
	Vector3 normal;
};

#endif // HITINFO_H
