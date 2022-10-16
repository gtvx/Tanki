#ifndef CONICAREATARGETSYSTEMBOT_H
#define CONICAREATARGETSYSTEMBOT_H

#include <qglobal.h>

class Body;
class Vector3;
class ICollisionDetector;
class ITargetValidator;
class GunPredicate;
class RayIntersection;
class ConicAreaGunPredicateBot;


class ConicAreaTargetSystemBot
{
	double range;
	double halfConeAngle;
	int numRays;
	int numSteps;

	ICollisionDetector *collisionDetector;
	ITargetValidator *targetValidator;
	ConicAreaGunPredicateBot *predicate;
	RayIntersection *intersection;
	QHash<Body*, double> *distanceByTarget;

public:
	ConicAreaTargetSystemBot(double range, double coneAngle, int numRays, int numSteps, ICollisionDetector *collisionDetector, ITargetValidator *targetValidator);
	~ConicAreaTargetSystemBot();

	void getTargets(Body *shooter, double barrelLength, double fireOriginOffsetCoeff, Vector3 *barrelOrigin, Vector3 *gunDir, Vector3 *gunRotationAxis, QVector<Body*> *targetBodies);

private:
	void processSector(Vector3 *rayOrigin, Vector3 *gunDir, Vector3 *rotationAxis, int numRays, double angleStep);
	void processRay(Vector3 *rayOrigin, Vector3 *rayDirection, double rayLength);
};


#endif // CONICAREATARGETSYSTEMBOT_H
