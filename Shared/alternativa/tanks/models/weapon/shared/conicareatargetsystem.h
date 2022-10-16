#ifndef CONICAREATARGETSYSTEM_H
#define CONICAREATARGETSYSTEM_H

#include <qglobal.h>

class Body;
class Vector3;
class ICollisionDetector;
class ITargetValidator;
class GunPredicate;
class RayIntersection;
class ConicAreaGunPredicate;


class ConicAreaTargetSystem
{
	double range;
	double halfConeAngle;
	int numRays;
	int numSteps;

	ICollisionDetector *collisionDetector;
	ITargetValidator *targetValidator;
	ConicAreaGunPredicate *predicate;
	RayIntersection *intersection;
	QHash<Body*, double> *distanceByTarget;

public:
	ConicAreaTargetSystem(double range, double coneAngle, int numRays, int numSteps, ICollisionDetector *collisionDetector, ITargetValidator *targetValidator);
	~ConicAreaTargetSystem();

	void getTargets(Body *shooter, double barrelLength, double fireOriginOffsetCoeff, Vector3 *barrelOrigin, Vector3 *gunDir, Vector3 *gunRotationAxis, QVector<Body*> *targetBodies, QVector<double> *targetDistances);

private:
	void processSector(Vector3 *rayOrigin, Vector3 *gunDir, Vector3 *rotationAxis, int numRays, double angleStep);
	void processRay(Vector3 *rayOrigin, Vector3 *rayDirection, double rayLength);
};

#endif // CONICAREATARGETSYSTEM_H
