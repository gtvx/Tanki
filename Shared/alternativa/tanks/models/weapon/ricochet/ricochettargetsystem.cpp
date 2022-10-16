#include "ricochettargetsystem.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/tanks/models/weapon/shared/ICommonTargetEvaluator.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/math/matrix3.h"



RicochetTargetSystem::RicochetTargetSystem(double angleUp,
										   int numRaysUp,
										   double angleDown,
										   int numRaysDown,
										   double range,
										   ICollisionDetector *collisionDetector,
										   ICommonTargetEvaluator *targetEvaluator)
{
	this->init(angleUp, numRaysUp, angleDown, numRaysDown, range, collisionDetector, targetEvaluator);
}

void RicochetTargetSystem::init(double angleUp,
								int numRaysUp,
								double angleDown,
								int numRaysDown,
								double range,
								ICollisionDetector *collisionDetector,
								ICommonTargetEvaluator *targetEvaluator)
{
	this->angleUp = angleUp;
	this->numRaysUp = numRaysUp;
	this->angleDown = angleDown;
	this->numRaysDown = numRaysDown;
	this->range = range;
	this->collisionDetector = collisionDetector;
	this->targetEvaluator = targetEvaluator;
}

bool RicochetTargetSystem::getShotDirection(const Vector3 *origin, const Vector3 *gunDir, const Vector3 *gunRotationAxis, Body *shooterBody, Vector3 *result)
{
	this->shooterBody = shooterBody;
	this->maxPriority = 0;
	this->targetDistance = this->range + 1;
	this->checkDirection(origin, gunDir, 0, result);
	this->checkSector(origin, gunDir, gunRotationAxis, (this->angleUp / this->numRaysUp), this->numRaysUp, result);
	this->checkSector(origin, gunDir, gunRotationAxis, (-(this->angleDown) / this->numRaysDown), this->numRaysDown, result);
	this->shooterBody = nullptr;
	return this->targetDistance < (this->range + 1);
}





void RicochetTargetSystem::checkDirection(const Vector3 *origin, const Vector3 *direction, double angle, Vector3 *result)
{
	this->ricochetCount = 0;
	double remainingDistance = this->range;
	int collisionGroup = CollisionGroup::WEAPON;

	Vector3 currOrigin;
	Vector3 currDirection;

	currOrigin.copy(origin);
	currDirection.copy(direction);

	RayIntersection rayHit;

	while (remainingDistance > 0)
	{
		if (this->collisionDetector->intersectRay(&currOrigin, &currDirection, collisionGroup, remainingDistance, this, &rayHit))
		{
			remainingDistance = remainingDistance - rayHit.t;
			if (remainingDistance < 0)
			{
				remainingDistance = 0;
			}
			Body *body = rayHit.primitive->body;
			if (((!(body == nullptr)) && (!(body == this->shooterBody))))
			{
				double distance = this->range - remainingDistance;
				double targetPriority = this->targetEvaluator->getTargetPriority(body, distance, angle);
				if (targetPriority > this->maxPriority)
				{
					this->maxPriority = targetPriority;
					this->targetDistance = distance;
					result->copy(direction);
				}
				return;
			}
			this->ricochetCount++;
			const Vector3 *normal = &rayHit.normal;
			currDirection.addScaled((-2 * currDirection.dot(normal)), normal);
			currOrigin.copy(&rayHit.pos);
			currOrigin.addScaled(0.1, normal);
		}
		else
		{
			return;
		}
	}
}

void RicochetTargetSystem::checkSector(const Vector3 *origin, const Vector3 *gunDir, const Vector3 *gunRotationAxis, double angleStep, int numRays, Vector3 *result)
{
	Vector3 dir;
	dir.copy(gunDir);
	Matrix3 matrix;
	matrix.fromAxisAngle(gunRotationAxis, angleStep);
	if (angleStep < 0)
	{
		angleStep = -angleStep;
	}
	double angle = angleStep;
	int i = 0;
	while (i < numRays)
	{
		dir.transformBy3(&matrix);
		this->checkDirection(origin, &dir, angle, result);
		i++;
		angle = angle + angleStep;
	}
}

bool RicochetTargetSystem::considerBody(Body *body)
{
	return !(this->shooterBody == body && this->ricochetCount == 0);
}
