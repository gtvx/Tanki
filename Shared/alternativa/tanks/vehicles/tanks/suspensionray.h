#ifndef SUSPENSIONRAY_H
#define SUSPENSIONRAY_H

#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/math/vector3.h"

class Body;
class SuspensionData;
class RayPredicate;


class SuspensionRay
{
	Body *body;
	Vector3 relPos;
	Vector3 relDir;
	Vector3 worldPos;
	Vector3 worldDir;
	Vector3 globalOrigin;
	bool lastCollided;

	RayIntersection lastIntersection;
	double prevDisplacement = 0;
	RayPredicate *predicate;

public:
	int collisionGroup;
	SuspensionRay(Body *body, Vector3 *relPos, Vector3 *relDir);
	~SuspensionRay();
	bool calculateIntersection(double maxLength);
	void addForce(double dt, double throttle, double maxSpeed, int slipTerm, double springCoeff, SuspensionData *data, bool fwdBrake);
	const Vector3* getGlobalOrigin() const { return &this->worldPos; }
};

#endif // SUSPENSIONRAY_H
