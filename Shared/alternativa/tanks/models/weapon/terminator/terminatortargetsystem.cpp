#include "terminatortargetsystem.h"
#include "terminatortargetsystem.h"
#include "alternativa/physics/collision/IRayCollisionPredicate.h"
#include "TerminatorShotResult.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "alternativa/tanks/models/ctf/ICTFModel.h"
#include "listvector3.h"
#include <QSet>

static const double FLAG_CARRIER_BONUS = 10;
static const double DISTANCE_WEIGHT = 0.65;
static const double BASE_DISTANCE = 20000;
static const double MAX_PRIORITY = 1000;
static const int COLLISION_GROUP = 16;
static const double MAX_RAY_LENGTH = 10000000000;


class TerminatorMultybodyCollisionPredicate : public IRayCollisionPredicate
{
public:
	QSet<Body*> bodies;
	Body *shooterBody;

	bool considerBody(Body *body) override
	{
		return !(shooterBody == body || bodies.contains(body));
	}
};



TerminatorTargetSystem::TerminatorTargetSystem()
{
	multybodyPredicate = new TerminatorMultybodyCollisionPredicate();
	currHitPoints = new ListVector3;
}

TerminatorTargetSystem::~TerminatorTargetSystem()
{
	delete multybodyPredicate;
	delete currHitPoints;
}

void TerminatorTargetSystem::setParams(ICollisionDetector *collisionDetector,
									   double upAngle,
									   int upRaysNum,
									   double downAngle,
									   int downRaysNum,
									   double weakeningCoeff,
									   ICTFModel *ctfModel)
{
	this->collisionDetector = collisionDetector;
	this->upAngleStep = upAngle / upRaysNum;
	this->upRaysNum = upRaysNum;
	this->downAngleStep = downAngle / downRaysNum;
	this->downRaysNum = downRaysNum;
	this->weakeningCoeff = weakeningCoeff;
	this->ctfModel = ctfModel;
	this->maxAngle = (upAngle > downAngle) ? upAngle : downAngle;
}

void TerminatorTargetSystem::getTargets(TankData *shooterData,
										const Vector3 *barrelOrigin,
										const Vector3 *baseDir,
										const Vector3 *rotationAxis,
										QHash<Body*, TankData*> *tanks,
										TerminatorShotResult *shotResult)
{
	shotResult->targets.clear();
	shotResult->hitPoints.clear();
	this->bestEffectivity = 0;
	this->bestDirectionIndex = 10000;
	this->maxDirectionPriority = 0;
	this->checkAllDirections(shooterData, barrelOrigin, baseDir, rotationAxis, tanks, shotResult);
	if (this->bestEffectivity == 0)
	{
		this->collectTargetsAlongRay(shooterData, barrelOrigin, baseDir, tanks, shotResult);
	}
	this->multybodyPredicate->bodies.clear();
	this->currTargets.clear();
	this->currHitPoints->clear();
}



void TerminatorTargetSystem::checkAllDirections(TankData *shooterData,
												const Vector3 *barrelOrigin,
												const Vector3 *baseDir,
												const Vector3 *rotationAxis,
												QHash<Body*, TankData*> *tanks,
												TerminatorShotResult *shotResult)
{
	this->checkDirection(barrelOrigin, baseDir, 0, 0, shooterData, tanks, shotResult);

	Matrix3 rotationMatrix;
	rotationMatrix.fromAxisAngle(rotationAxis, this->upAngleStep);
	Vector3 dir;
	dir.copy(baseDir);
	double angle = 0;
	int i = 1;
	while (i <= this->upRaysNum)
	{
		angle = angle + this->upAngleStep;
		dir.transformBy3(&rotationMatrix);
		this->checkDirection(barrelOrigin, &dir, angle, i, shooterData, tanks, shotResult);
		i++;
	}
	rotationMatrix.fromAxisAngle(rotationAxis, -(this->downAngleStep));
	dir.copy(baseDir);
	angle = 0;
	i = 1;
	while (i <= this->downRaysNum)
	{
		angle = angle + this->downAngleStep;
		dir.transformBy3(&rotationMatrix);
		this->checkDirection(barrelOrigin, &dir, angle, i, shooterData, tanks, shotResult);
		i++;
	}
}

void TerminatorTargetSystem::checkDirection(const Vector3 *barrelOrigin,
											const Vector3 *barrelDir,
											double angle,
											int dirIndex,
											TankData *shooterData,
											QHash<Body*, TankData*> *tanks,
											TerminatorShotResult *shotResult)
{
	this->currTargets.clear();
	this->currHitPoints->clear();

	double effectivity = this->evaluateDirection(barrelOrigin, barrelDir, shooterData, tanks, &this->currTargets, this->currHitPoints);
	if (effectivity > 0)
	{
		Vector3 v;
		v.diff(this->currHitPoints->at(0), barrelOrigin);
		double distance = v.length();
		double directionPriority = this->getDirectionPriority(angle, distance);
		if (((effectivity > this->bestEffectivity) || ((effectivity == this->bestEffectivity) && (directionPriority > this->maxDirectionPriority))))
		{
			this->bestEffectivity = effectivity;
			this->bestDirectionIndex = dirIndex;
			this->maxDirectionPriority = directionPriority;
			shotResult->dir.copy(barrelDir);
			int len = this->currTargets.length();

			for (int i = 0; i < len; i++)
				shotResult->targets.append(this->currTargets[i]);

			len = this->currHitPoints->length();

			for (int i = 0; i < len; i++)
				shotResult->hitPoints.add(this->currHitPoints->at(i));
		}
	}
}



double TerminatorTargetSystem::evaluateDirection(const Vector3 *barrelOrigin,
												 const Vector3 *barrelDir,
												 TankData *shooterData,
												 QHash<Body*, TankData*> *tanks,
												 QVector<TankData*> *targets,
												 ListVector3 *hitPoints)
{
	this->multybodyPredicate->bodies.clear();
	this->multybodyPredicate->shooterBody = shooterData->tank;
	Vector3 origin;
	origin.copy(barrelOrigin);
	double effectivity = 0;
	double effectivityCoeff = 1;
	bool firstTarget = true;

	RayIntersection intersection;

	while (this->collisionDetector->intersectRay(&origin,
												 barrelDir,
												 COLLISION_GROUP,
												 MAX_RAY_LENGTH,
												 this->multybodyPredicate,
												 &intersection))
	{
		Body *body = intersection.primitive->body;
		if (body == nullptr)
		{
			hitPoints->add(&intersection.pos);
			break;
		}
		TankData *targetData = tanks->value(body);
		if (targetData != nullptr)
		{
			bool targetIsEnemy = ((shooterData->teamType == BattleTeamType::NONE) || (!(shooterData->teamType == targetData->teamType)));
			if (firstTarget)
			{
				if ((targetData->health > 0) && !targetIsEnemy)
				{
					return 0;
				}
				firstTarget = false;
			}
			if (targetData->health > 0)
			{
				if (targetIsEnemy)
				{
					if (((!(this->ctfModel == nullptr)) && (this->ctfModel->isFlagCarrier(targetData))))
					{
						effectivity = effectivity + (FLAG_CARRIER_BONUS * effectivityCoeff);
					}
					else
					{
						effectivity = effectivity + effectivityCoeff;
					}
				}
				else
				{
					effectivity = effectivity - effectivityCoeff;
				}
			}
			effectivityCoeff = effectivityCoeff * this->weakeningCoeff;
			targets->append(targetData);
			hitPoints->add(&intersection.pos);
		}
		this->multybodyPredicate->bodies.insert(body);
		origin.copy(&intersection.pos);
	}
	return effectivity;
}

void TerminatorTargetSystem::collectTargetsAlongRay(TankData *shooterData,
													const Vector3 *barrelOrigin,
													const Vector3 *barrelDir,
													QHash<Body*, TankData*> *tanks,
													TerminatorShotResult *shotResult)
{
	shotResult->hitPoints.clear();
	shotResult->targets.clear();
	shotResult->dir.copy(barrelDir);
	this->multybodyPredicate->bodies.clear();
	this->multybodyPredicate->shooterBody = shooterData->tank;

	Vector3 origin;
	origin.copy(barrelOrigin);

	RayIntersection intersection;

	while (this->collisionDetector->intersectRay(&origin, barrelDir, COLLISION_GROUP, MAX_RAY_LENGTH, this->multybodyPredicate, &intersection))
	{
		Body *body = intersection.primitive->body;
		if (body == nullptr)
		{
			shotResult->hitPoints.add(&intersection.pos);
			break;
		}
		TankData *targetData = tanks->value(body);
		if (targetData != nullptr)
		{
			shotResult->targets.append(targetData);
			shotResult->hitPoints.add(&intersection.pos);
		}
		this->multybodyPredicate->bodies.insert(body);
		origin.copy(&intersection.pos);
	}
}

double TerminatorTargetSystem::getDirectionPriority(double angle, double distance)
{
	return (MAX_PRIORITY - (((DISTANCE_WEIGHT * distance) / BASE_DISTANCE) + (((1 - DISTANCE_WEIGHT) * angle) / this->maxAngle)));
}
