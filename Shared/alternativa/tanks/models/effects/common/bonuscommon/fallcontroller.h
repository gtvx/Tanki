#ifndef FALLCONTROLLER_H
#define FALLCONTROLLER_H

#include "battlebonusstate.h"
#include "alternativa/math/vector3.h"
#include "alternativa/math/matrix3.h"

class ParaBonus2;
class Object3D;

class FallController
{
	Matrix3 interpolatedMatrix;
	Matrix3 interpolatedParachuteMatrix;
	Vector3 interpolatedVector;
	Vector3 interpolatedParachuteVector;

	BattleBonusState oldState;
	BattleBonusState oldStateParachute;
	BattleBonusState newState;
	BattleBonusState newStateParachute;
	BattleBonusState interpolatedState;
	BattleBonusState interpolatedParachuteState;

	ParaBonus2 *battleBonus;
	double minPivotZ;
	double time;
	double fallSpeed;
	double t0;
	double x;
	double y;

public:
	FallController(ParaBonus2 *battleBonus);
	void init(const Vector3 *spawnPosition, double fallSpeed, double minPivotZ, double t0, double startTime, double startingAngleZ);
	void start();
	void runBeforePhysicsUpdate(double dt);

private:
	void updateTrigger();

public:
	void interpolatePhysicsState(double interpolationCoeff);
	void render();

private:
	void setObjectTransform(Object3D *object, double objectOffset, const Vector3 *offsetVector);
};


#endif // FALLCONTROLLER_H
