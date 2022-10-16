#include "commontargetsystembot.h"
#include "alternativa/tanks/models/weapon/shared/ICommonTargetEvaluator.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/tanks/models/weapon/common/HitInfo.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/physics/collision/IRayCollisionPredicate.h"


#define COLLISION_GROUP CollisionGroup::WEAPON


class CommonGunPredicateBot : public IRayCollisionPredicate
{
public:
	Body* shooter;

	bool considerBody(Body *body) override
	{
		return this->shooter != body;
	}
};


CommonTargetSystemBot::CommonTargetSystemBot(double maxDistance, double maxAngleUp, int numRaysUp, double maxAngleDown, int numRaysDown, ICollisionDetector *collisionDetector, ICommonTargetEvaluator *targetEvaluator)
{
	this->bestTarget = nullptr;
	this->maxDistance = maxDistance;
	this->maxAngleUp = maxAngleUp;
	this->numRaysUp = numRaysUp;
	this->maxAngleDown = maxAngleDown;
	this->numRaysDown = numRaysDown;
	this->collisionDetector = collisionDetector;
	this->targetEvaluator = targetEvaluator;
	this->predicate = new CommonGunPredicateBot;
	this->intersection = new RayIntersection;
	this->bestDirection = new Vector3;
}

CommonTargetSystemBot::~CommonTargetSystemBot()
{
	delete this->predicate;
	delete this->intersection;
	delete this->bestDirection;
}

bool CommonTargetSystemBot::getTarget(Vector3 *barrelOrigin, Vector3 *gunDirection, Vector3 *gunRotationAxis, Body *shooterBody, HitInfo *hitInfo)
{
	this->maxPriority = 0;
	this->hitDistance = this->maxDistance + 1.;
	this->bestTarget = nullptr;

	predicate->shooter = shooterBody;


	Vector3 normal;

	//if (this->collisionDetector->intersectRay(barrelOrigin, gunDirection, COLLISION_GROUP, this->maxDistance, this->predicate, intersection))
	if (this->collisionDetector->intersectRayNecessarilyDynamic(barrelOrigin, gunDirection, COLLISION_GROUP, this->maxDistance, predicate, intersection))
	{
		normal.copy(&intersection->normal);
		bestDirection->copy(gunDirection);
		this->hitDistance = intersection->t;
		Body *body = intersection->primitive->body;
		if (body != nullptr)
		{
			this->bestTarget = body;
			this->maxPriority = this->targetEvaluator->getTargetPriority(body, this->hitDistance, 0);
		}
	}

	if (this->numRaysUp > 0)
	{
		this->checkSector(barrelOrigin, gunDirection, gunRotationAxis, this->numRaysUp, this->maxAngleUp / this->numRaysUp);
	}

	if (this->numRaysDown > 0)
	{
		this->checkSector(barrelOrigin, gunDirection, gunRotationAxis, this->numRaysDown, -this->maxAngleDown / this->numRaysDown);
	}

	//this->predicate.shooter = null;
	if (this->hitDistance <= this->maxDistance)
	{
		hitInfo->distance = this->hitDistance;
		hitInfo->normal.copy(&normal);
		hitInfo->direction.copy(bestDirection);
		hitInfo->position.copy(barrelOrigin);
		hitInfo->position.addScaled(this->hitDistance, bestDirection);
		hitInfo->body = this->bestTarget;
		this->bestTarget = nullptr;
		return true;
	}

	return false;
}


void CommonTargetSystemBot::checkSector(Vector3 *barrelOrigin, Vector3 *gunDirection, Vector3 *barrelRotationAxis, int raysNum, double angleStep)
{
	Matrix3 matrix;
	matrix.fromAxisAngle(barrelRotationAxis, angleStep);

	if (angleStep < 0)
		angleStep = -angleStep;

	Vector3 rayDir2;
	rayDir2.copy(gunDirection);

	double angle = 0.;
	for (int i = 1; i <= raysNum; i++)
	{
		angle += angleStep;
		Vector3 rayDir1;
		rayDir1.copy(&rayDir2);
		matrix.transformVector(&rayDir1, &rayDir2);

		//if (this->collisionDetector->intersectRay(barrelOrigin, &rayDir2, COLLISION_GROUP, this->maxDistance, this->predicate, intersection))
		if (this->collisionDetector->intersectRayNecessarilyDynamic(barrelOrigin, &rayDir2, COLLISION_GROUP, this->maxDistance, this->predicate, intersection))
		{
			Body *body = intersection->primitive->body;
			if (body != nullptr)
			{
				double targetPriority = this->targetEvaluator->getTargetPriority(body, intersection->t, angle);
				if (targetPriority > this->maxPriority)
				{
					this->maxPriority = targetPriority;
					this->bestTarget = body;
					this->bestDirection->copy(&rayDir2);
					this->hitDistance = intersection->t;
				}
			}
		}
	}
}

