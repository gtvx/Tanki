#include "conicareatargetsystembot.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "ITargetValidator.h"
#include "alternativa/physics/collision/IRayCollisionPredicate.h"
#include "mymath.h"
#include "_global.h"
#include <QVector>
#include <QHashIterator>
#include <QHash>


#define COLLISION_GROUP CollisionGroup::WEAPON


class ConicAreaGunPredicateBot : public IRayCollisionPredicate
{
	QHash<Body*, bool> targets;
	QHash<Body*, bool> invalidTargets;

public:
	Body *shooter;

	bool considerBody(Body *body) override
	{
		return (this->shooter != body) && (this->targets.value(body) == false) && (this->invalidTargets.value(body) == false);
	}

	void addTarget(Body *body)
	{
		this->targets.insert(body, true);
	}

	void addInvalidTarget(Body *body)
	{
		this->invalidTargets.insert(body, true);
	}

	void clearTargets()
	{
		this->targets.clear();
	}

	void clearInvalidTargets()
	{
		this->invalidTargets.clear();
	}
};





ConicAreaTargetSystemBot::ConicAreaTargetSystemBot(double range, double coneAngle, int numRays, int numSteps, ICollisionDetector *collisionDetector, ITargetValidator *targetValidator)
{
	this->range = range;
	this->halfConeAngle = 0.5 * coneAngle;
	this->numRays = numRays;
	this->numSteps = numSteps;
	this->collisionDetector = collisionDetector;
	this->targetValidator = targetValidator;
	this->predicate = new ConicAreaGunPredicateBot;
	this->intersection = new RayIntersection;
	this->distanceByTarget = new QHash<Body*, double>;
}

ConicAreaTargetSystemBot::~ConicAreaTargetSystemBot()
{
	delete this->predicate;
	delete this->intersection;
	delete this->distanceByTarget;
}

void ConicAreaTargetSystemBot::getTargets(Body *shooter, double barrelLength, double fireOriginOffsetCoeff, Vector3 *barrelOrigin, Vector3 *gunDir, Vector3 *gunRotationAxis, QVector<Body*> *targetBodies)
{
	this->predicate->shooter = shooter;

	double fireOriginOffset = fireOriginOffsetCoeff * barrelLength;
	double extraDamageAreaRange = barrelLength - fireOriginOffset;

	if (this->collisionDetector->intersectRayNecessarilyDynamic(barrelOrigin, gunDir, COLLISION_GROUP, barrelLength, this->predicate, this->intersection))
		if (this->intersection->primitive->body == nullptr)
			return;

	Vector3 xAxis;
	xAxis.copy(gunRotationAxis);
	Vector3 muzzlePos;
	muzzlePos.copy(barrelOrigin);
	muzzlePos.addScaled(fireOriginOffset, gunDir);

	this->processRay(&muzzlePos, gunDir, this->range + extraDamageAreaRange);

	Matrix3 rotationMatrix;
	rotationMatrix.fromAxisAngle(gunDir, MyMath::PI / this->numSteps);
	double angleStep = this->halfConeAngle / this->numRays;

	for (int i = 0; i < this->numSteps; i++)
	{
		this->processSector(&muzzlePos, gunDir, &xAxis, this->numRays, angleStep);
		this->processSector(&muzzlePos, gunDir, &xAxis, this->numRays, -angleStep);
		xAxis.transformBy3(&rotationMatrix);
	}



	{
		QHashIterator<Body*, double> i(*distanceByTarget);
		while (i.hasNext()) {
			i.next();

			targetBodies->append(i.key());
			double distance = i.value() - extraDamageAreaRange;
			if (distance < 0.)
				distance = 0.;
		}
	}

	this->predicate->shooter = nullptr;
	this->predicate->clearInvalidTargets();
	distanceByTarget->clear();
}

void ConicAreaTargetSystemBot::processSector(Vector3 *rayOrigin, Vector3 *gunDir, Vector3 *rotationAxis, int numRays, double angleStep)
{
	double rayAngle = 0;
	for (int i = 0; i < numRays; i++)
	{
		rayAngle += angleStep;
		Matrix3 matrix;
		matrix.fromAxisAngle(rotationAxis, rayAngle);
		Vector3 rayDir;
		matrix.transformVector(gunDir, &rayDir);
		this->processRay(rayOrigin, &rayDir, this->range);
	}
}

void ConicAreaTargetSystemBot::processRay(Vector3 *rayOrigin, Vector3 *rayDirection, double rayLength)
{
	Vector3 origin;
	origin.copy(rayOrigin);
	double distance = 0;
	while (rayLength > 0.1)
	{
		if (this->collisionDetector->intersectRayNecessarilyDynamic(&origin, rayDirection, COLLISION_GROUP, rayLength, this->predicate, this->intersection))
		{

			Body *body = this->intersection->primitive->body;
			if (body == nullptr)
				break;

			origin.addScaled(this->intersection->t, rayDirection);
			distance += this->intersection->t;
			if (this->targetValidator->isValidTarget(body))
			{
				this->predicate->addTarget(body);

				if (this->distanceByTarget->contains(body) == false)
				{
					this->distanceByTarget->insert(body, distance);
				}
				else
				{
					double d = this->distanceByTarget->value(body);
					if (d > distance)
					{
						this->distanceByTarget->insert(body, distance);
					}
				}
			}
			else
			{
				this->predicate->addInvalidTarget(body);
			}
			rayLength = rayLength - this->intersection->t;
		}
		else
		{
			break;
		}
	}
	this->predicate->clearTargets();
}
