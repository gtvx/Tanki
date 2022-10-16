#ifndef LANDINGCONTROLLER_H
#define LANDINGCONTROLLER_H

#include "alternativa/math/vector3.h"
#include "landingstate.h"

class ParaBonus2;

class LandingController
{
	ParaBonus2 *bonus;
	Vector3 normal;
	Vector3 pivot;
	Vector3 r;
	Vector3 axis;
	double angle;
	LandingState oldState;
	LandingState newState;
	LandingState interpolatedState;

public:
	LandingController(ParaBonus2 *bonus);
	void init(Vector3 *pivot, Vector3 *normal);
	void start();
	void runBeforePhysicsUpdate(double dt);

private:
	void updateTrigger();

public:
	void interpolatePhysicsState(double interpolationCoeff);
	void render();
};

#endif // LANDINGCONTROLLER_H
