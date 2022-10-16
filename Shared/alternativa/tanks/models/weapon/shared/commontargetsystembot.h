#ifndef COMMONTARGETSYSTEMBOT_H
#define COMMONTARGETSYSTEMBOT_H


class ICollisionDetector;
class ICommonTargetEvaluator;
class Vector3;
class Body;
class HitInfo;
class CommonGunPredicateBot;
class RayIntersection;


class CommonTargetSystemBot
{
	ICollisionDetector *collisionDetector;
	ICommonTargetEvaluator *targetEvaluator;
	Body *bestTarget;
	CommonGunPredicateBot *predicate;
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
	CommonTargetSystemBot(double maxDistance, double maxAngleUp, int numRaysUp, double maxAngleDown, int numRaysDown, ICollisionDetector *collisionDetector, ICommonTargetEvaluator *targetEvaluator);
	~CommonTargetSystemBot();

	bool getTarget(Vector3 *barrelOrigin, Vector3 *gunDirection, Vector3 *gunRotationAxis, Body *shooterBody, HitInfo *hitInfo);
	void setMaxDistance(double value) { this->maxDistance = value; }
	double getMaxDistance() { return this->maxDistance; }

private:
	void checkSector(Vector3 *barrelOrigin, Vector3 *gunDirection, Vector3 *barrelRotationAxis, int raysNum, double angleStep);
};


#endif // COMMONTARGETSYSTEMBOT_H
