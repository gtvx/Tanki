#ifndef LANDINGSTATE_H
#define LANDINGSTATE_H

#include "alternativa/math/vector3.h"
#include "alternativa/math/quaternion.h"

class LandingState
{
public:

	Vector3 position;
	Quaternion orientation;

	LandingState();

	void interpolate(LandingState *a, LandingState *b, double t);
	void copy(LandingState *src);
};

#endif // LANDINGSTATE_H
