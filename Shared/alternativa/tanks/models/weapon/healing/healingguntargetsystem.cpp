#include "healingguntargetsystem.h"
#include "alternativa/math/vector3.h"
#include "alternativa/physics/collision/IRayCollisionPredicate.h"
#include "alternativa/tanks/models/weapon/healing/HealingGunData.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "mymath.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "flash/geom/matrix3d.h"


static const int collisionGroup = 16;
static const double LOCKING_RANGE_DELTA = 50;
static const double angleWeight = 0.6;
static const double distanceWeight = 0.4;


class HealingGunPredicate : public IRayCollisionPredicate
{
public:
	Body* healer;

	bool considerBody(Body *body) override
	{
		return this->healer != body;
	}
};


HealingGunTargetSystem::HealingGunTargetSystem()
{
	this->predicate = new HealingGunPredicate;
}


HealingGunTargetSystem::~HealingGunTargetSystem()
{
	delete this->predicate;
}

TankData* HealingGunTargetSystem::getTarget(TankData *healerData,
											HealingGunData *gunData,
											double barrelLength,
											Vector3 *barrelOrigin,
											Vector3 *gunDir,
											Vector3 *xAxis,
											int numRays,
											double numSteps,
											TanksCollisionDetector *collisionDetector,
											QHash<Body*, TankData*> &tankDatas)
{
	RayIntersection intersection;


	if (collisionDetector->intersectRayWithStatic(barrelOrigin, gunDir, CollisionGroup::STATIC, barrelLength, nullptr, &intersection))
	{
		return nullptr;
	}


	Vector3 muzzlePosGlobal;

	muzzlePosGlobal.x = (barrelOrigin->x + (barrelLength * gunDir->x));
	muzzlePosGlobal.y = (barrelOrigin->y + (barrelLength * gunDir->y));
	muzzlePosGlobal.z = (barrelOrigin->z + (barrelLength * gunDir->z));
	this->bestTarget = nullptr;
	this->maxTargetPriority = 0;
	this->shooterTeamType = healerData->teamType;
	double lockingRangeSqr = gunData->maxRadius - LOCKING_RANGE_DELTA;
	lockingRangeSqr = lockingRangeSqr * lockingRangeSqr;
	Vector3 _xAxis;
	_xAxis.x = xAxis->x;
	_xAxis.y = xAxis->y;
	_xAxis.z = xAxis->z;
	this->predicate->healer = healerData->tank;

	if (collisionDetector->intersectRay(&muzzlePosGlobal, gunDir, collisionGroup, gunData->maxRadius, this->predicate, &intersection))
	{
		Body *targetBody = intersection.primitive->body;
		if (targetBody != nullptr)
		{
			this->checkTarget(targetBody, tankDatas, gunData, lockingRangeSqr, &muzzlePosGlobal, gunDir, collisionDetector);
		}
	}

	Matrix3 rotationMatrix;
	rotationMatrix.fromAxisAngle(gunDir, MyMath::PI / numSteps);
	double angleStep = gunData->lockAngle / numRays;
	int i = 0;
	while (i < numSteps)
	{
		this->checkSector(gunData, lockingRangeSqr, &muzzlePosGlobal, gunDir, &_xAxis, numRays, angleStep, collisionDetector, tankDatas, 0);
		this->checkSector(gunData, lockingRangeSqr, &muzzlePosGlobal, gunDir, &_xAxis, numRays, -angleStep, collisionDetector, tankDatas, numSteps);
		_xAxis.transformBy3(&rotationMatrix);
		i++;
	}
	TankData *target = this->bestTarget;
	this->bestTarget = nullptr;
	this->predicate->healer = nullptr;

	this->checkedTargets.clear();
	return target;
}



bool HealingGunTargetSystem::validateTarget(TankData *healerData, TankData *targetData, HealingGunData *gunData, Vector3 *gunDir, Vector3 *barrelOrigin, double barrelLength, TanksCollisionDetector *collisionDetector)
{
	RayIntersection intersection;

	if (collisionDetector->intersectRayWithStatic(barrelOrigin, gunDir, CollisionGroup::STATIC, barrelLength, nullptr, &intersection))
	{
		return false;
	}

	Vector3 muzzlePosGlobal;

	muzzlePosGlobal.x = barrelOrigin->x + (barrelLength * gunDir->x);
	muzzlePosGlobal.y = barrelOrigin->y + (barrelLength * gunDir->y);
	muzzlePosGlobal.z = barrelOrigin->z + (barrelLength * gunDir->z);


	Matrix4 m;
	m.setMatrix(targetData->tank->skin->turretMesh->position.x,
				targetData->tank->skin->turretMesh->position.y,
				targetData->tank->skin->turretMesh->position.z,
				targetData->tank->skin->turretMesh->rotation.x,
				targetData->tank->skin->turretMesh->rotation.y,
				targetData->tank->skin->turretMesh->rotation.z);
	Matrix3D m3;
	m3.set(m.ma, m.me, m.mi, 0, m.mb, m.mf, m.mj, 0, m.mc, m.mg, m.mk, 0, m.md, m.mh, m.ml, 1);

	const double vin[3] = {0, 0, 0};
	double _vout[3];


	m3.transformVectors_1(vin, _vout);

	//Vector3 vin;
	//vin.reset();
	Vector3 vout;
	vout.x = _vout[0];
	vout.y = _vout[1];
	vout.z = _vout[2];

	//targetData->tank->skin->turretMesh->matrix.transformVector(&vin, &vout);

	double dx = vout.x - muzzlePosGlobal.x;
	double dy = vout.y - muzzlePosGlobal.y;
	double dz = vout.z - muzzlePosGlobal.z;
	double d = (dx * dx) + (dy * dy) + (dz * dz);
	if (d > (gunData->maxRadius * gunData->maxRadius))
	{
		return false;
	}
	Vector3 rayDir;
	rayDir.x = dx;
	rayDir.y = dy;
	rayDir.z = dz;
	d = 1. / MyMath::sqrt(d);
	dx = (dx * d);
	dy = (dy * d);
	dz = (dz * d);
	double cos = (((dx * gunDir->x) + (dy * gunDir->y)) + (dz * gunDir->z));
	if (cos < gunData->maxAngleCos)
	{
		return false;
	}
	this->predicate->healer = healerData->tank;

	if (collisionDetector->intersectRay(&muzzlePosGlobal, &rayDir, CollisionGroup::WEAPON, 1, this->predicate, &intersection))
	{
		this->predicate->healer = nullptr;
		Body *body = intersection.primitive->body;
		if ((body == nullptr) || (!(body == targetData->tank)))
		{
			return false;
		}
	}
	return true;
}

void HealingGunTargetSystem::checkSector(HealingGunData *gunData,
										 double lockingRangeSqr,
										 const Vector3 *rayOrigin,
										 const Vector3 *gunDir,
										 const Vector3 *xAxis,
										 int raysNum,
										 double angleStep,
										 ICollisionDetector *collisionDetector,
										 QHash<Body*, TankData*> &tanks,
										 int step)
{
	(void)step;

	double angle = 0;
	int i = 1;
	while (i <= raysNum)
	{
		angle = angle + angleStep;
		Matrix3 matrix;
		Vector3 rayDir;
		matrix.fromAxisAngle(xAxis, angle);
		matrix.transformVector(gunDir, &rayDir);
		RayIntersection intersection;
		if (collisionDetector->intersectRay(rayOrigin, &rayDir, collisionGroup, gunData->maxRadius, this->predicate, &intersection))
		{
			Body *targetBody = intersection.primitive->body;
			if (targetBody != nullptr)
			{
				this->checkTarget(targetBody, tanks, gunData, lockingRangeSqr, rayOrigin, gunDir, collisionDetector);
			}
		}
		i++;
	}
}

void HealingGunTargetSystem::checkTarget(Body *targetBody,
										 QHash<Body*, TankData*> &tanks,
										 HealingGunData *gunData,
										 double lockingRangeSqr,
										 const Vector3 *rayOrigin,
										 const Vector3 *gunDir,
										 ICollisionDetector *collisionDetector)
{
	if (this->checkedTargets.contains(targetBody))
		return;
	this->checkedTargets.insert(targetBody);

	TankData *targetData = tanks.value(targetBody);
	if (targetData == nullptr || targetData->enabled == false)
	{
		return;
	}

	Matrix4 m;
	m.setMatrix(targetData->tank->skin->turretMesh->position.x,
				targetData->tank->skin->turretMesh->position.y,
				targetData->tank->skin->turretMesh->position.z,
				targetData->tank->skin->turretMesh->rotation.x,
				targetData->tank->skin->turretMesh->rotation.y,
				targetData->tank->skin->turretMesh->rotation.z);
	Matrix3D m3;
	m3.set(m.ma, m.me, m.mi, 0, m.mb, m.mf, m.mj, 0, m.mc, m.mg, m.mk, 0, m.md, m.mh, m.ml, 1);

	const double vin[3] = {0, 0, 0};
	double _vout[3];

	m3.transformVectors_1(vin, _vout);

	//Matrix3D *matrix = targetData->tank->skin->turretMesh->matrix;
	//matrix.transformVectors(this->vin, this->vout);

	//Vector3 vin;
	//vin.reset();
	Vector3 vout;
	vout.x = _vout[0];
	vout.y = _vout[1];
	vout.z = _vout[2];

	//targetData->tank->skin->turretMesh->matrix.transformVector(&vin, &vout);

	double dx = (vout.x - rayOrigin->x);
	double dy = (vout.y - rayOrigin->y);
	double dz = (vout.z - rayOrigin->z);
	double d = (dx * dx) + (dy * dy) + (dz * dz);
	if (d > lockingRangeSqr)
	{
		return;
	}
	double distanceSqr = d;
	d = (1. / MyMath::sqrt(d));
	dx = (dx * d);
	dy = (dy * d);
	dz = (dz * d);
	double angleCos = (((dx * gunDir->x) + (dy * gunDir->y)) + (dz * gunDir->z));
	if (angleCos <= gunData->maxAngleCos)
	{
		return;
	}
	Vector3 rayDir;
	rayDir.x = dx;
	rayDir.y = dy;
	rayDir.z = dz;
	RayIntersection intersection;
	if (collisionDetector->intersectRay(rayOrigin, &rayDir, collisionGroup, gunData->maxRadius, this->predicate, &intersection))
	{
		if (intersection.primitive->body != targetBody)
		{
			return;
		}
	}
	double priority = (this->getBaseTargetPriority(targetData) + ((distanceWeight * (lockingRangeSqr - distanceSqr)) / lockingRangeSqr)) + (angleWeight * angleCos);
	if (priority > this->maxTargetPriority)
	{
		this->maxTargetPriority = priority;
		this->bestTarget = targetData;
	}
}

double HealingGunTargetSystem::getBaseTargetPriority(TankData *targetData)
{
	if (this->shooterTeamType == BattleTeamType::NONE)
	{
		return 1;
	}
	if (this->shooterTeamType == targetData->teamType)
	{
		return (targetData->health == 10000) ? 1 : 3;
	}
	return 2;
}
