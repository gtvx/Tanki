#ifndef COMMONTARGETSYSTEM_H
#define COMMONTARGETSYSTEM_H

class ICollisionDetector;
class ICommonTargetEvaluator;
class Vector3;
class Body;
class HitInfo;
class CommonGunPredicate;
class RayIntersection;


class CommonTargetSystem
{
	ICollisionDetector *collisionDetector;
	ICommonTargetEvaluator *targetEvaluator;
	Body *bestTarget;
	CommonGunPredicate *predicate;
	RayIntersection *intersection;
	Vector3 *bestDirection;

	double maxDistance;
	double maxAngleUp;
	double maxAngleDown;
	double maxPriority;
	double hitDistance;

	int numRaysUp;
	int numRaysDown;


public:
	CommonTargetSystem(double maxDistance,
					   double maxAngleUp,
					   int numRaysUp,
					   double maxAngleDown,
					   int numRaysDown,
					   ICollisionDetector *collisionDetector,
					   ICommonTargetEvaluator *targetEvaluator);

	~CommonTargetSystem();

	bool getTarget(Vector3 *barrelOrigin, Vector3 *gunDirection, Vector3 *gunRotationAxis, Body *shooterBody, HitInfo *hitInfo);

private:
	void checkSector(Vector3 *barrelOrigin, Vector3 *gunDirection, Vector3 *barrelRotationAxis, int raysNum, double angleStep);
};

#endif // COMMONTARGETSYSTEM_H
