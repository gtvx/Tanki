#ifndef RICOCHETTARGETSYSTEM_H
#define RICOCHETTARGETSYSTEM_H

#include "alternativa/physics/collision/IRayCollisionPredicate.h"

class ICollisionDetector;
class ICommonTargetEvaluator;
class Vector3;
class Body;


class RicochetTargetSystem : public IRayCollisionPredicate
{
	double angleUp;
	double angleDown;
	double range;
	double targetDistance;
	double maxPriority;

	int numRaysDown;
	int numRaysUp;
	int ricochetCount;

	ICollisionDetector *collisionDetector;
	ICommonTargetEvaluator *targetEvaluator;
	Body *shooterBody;

public:
	RicochetTargetSystem(double angleUp,
						 int numRaysUp,
						 double angleDown,
						 int numRaysDown,
						 double range,
						 ICollisionDetector *collisionDetector,
						 ICommonTargetEvaluator *targetEvaluator);

	void init(double angleUp,
			  int numRaysUp,
			  double angleDown,
			  int numRaysDown,
			  double range,
			  ICollisionDetector *collisionDetector,
			  ICommonTargetEvaluator *targetEvaluator);


	bool getShotDirection(const Vector3 *origin,
						  const Vector3 *gunDir,
						  const Vector3 *gunRotationAxis,
						  Body *shooterBody,
						  Vector3 *result);


private:
	void checkDirection(const Vector3 *origin,
						const Vector3 *direction,
						double angle,
						Vector3 *result);

	void checkSector(const Vector3 *origin,
					 const Vector3 *gunDir,
					 const Vector3 *gunRotationAxis,
					 double angleStep,
					 int numRays,
					 Vector3 *result);

	bool considerBody(Body *body) override;
};

#endif // RICOCHETTARGETSYSTEM_H
