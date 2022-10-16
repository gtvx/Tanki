#include "collisionobject3dpositionprovider.h"
#include "alternativa/engine3d/core/object3d.h"
#include "mymath.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/physics/collisiongroup.h"

static double distanceBetween(const Vector3 *a, const Vector3 *b)
{
	double x = a->x - b->x;
	double y = a->y - b->y;
	double z = a->z - b->z;
	return MyMath::sqrt((x * x) + (y * y) + (z * z));
}

static const double MIN_DISTANCE = 20;
static const double SMOOTH = 0.2;


CollisionObject3DPositionProvider::CollisionObject3DPositionProvider(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	currentDistance = 0;
	localMuzzlePosition = new Vector3();
	barrelOrigin = new Vector3();
	direction = new Vector3();
}

void CollisionObject3DPositionProvider::init(Object3D *turret, const Vector3 *localMuzzlePosition, TanksCollisionDetector *collisionDetector, double maxDistance, double coeff)
{
	this->turret = turret;
	this->localMuzzlePosition->copy(localMuzzlePosition);
	this->collisionDetector = collisionDetector;
	this->maxDistance = maxDistance;
	this->coeff = coeff;
	this->currentDistance = 0;
}

void CollisionObject3DPositionProvider::initPosition(Object3D *object)
{
	this->calculateParameters();
	double v = this->maxDistance * this->coeff;
	RayIntersection intersection;
	if (this->collisionDetector->intersectRayWithStatic(barrelOrigin, direction, CollisionGroup::STATIC, this->maxDistance, nullptr, &intersection))
	{
		v = distanceBetween(barrelOrigin, &intersection.pos) * this->coeff;
	}
	double v2 = v - this->currentDistance;
	if (MyMath::abs(v2) <= MIN_DISTANCE)
	{
		this->currentDistance = v;
	}
	else
	{
		this->currentDistance = this->currentDistance + (v2 * SMOOTH);
	}
	object->position.x = (barrelOrigin->x + (direction->x * this->currentDistance));
	object->position.y = (barrelOrigin->y + (direction->y * this->currentDistance));
	object->position.z = (barrelOrigin->z + (direction->z * this->currentDistance));
}

void CollisionObject3DPositionProvider::updateObjectPosition(Object3D *object, GameCamera *camera, int value)
{
	(void)camera;
	(void)value;
	this->initPosition(object);
}

void CollisionObject3DPositionProvider::destroy()
{
	this->turret = nullptr;
	this->collisionDetector = nullptr;
	this->storeInPool();
}

void CollisionObject3DPositionProvider::calculateParameters()
{
	Matrix4 turretMatrix;
	turretMatrix.setMatrix(&this->turret->position, &this->turret->rotation);
	//turretAxisX.x = turretMatrix.a;
	//turretAxisX.y = turretMatrix.e;
	//turretAxisX.z = turretMatrix.i;
	direction->x = turretMatrix.b;
	direction->y = turretMatrix.f;
	direction->z = turretMatrix.j;
	Vector3 globalMuzzlePosition;
	turretMatrix.transformVector(this->localMuzzlePosition, &globalMuzzlePosition);
	double _loc1_ = this->localMuzzlePosition->y;
	barrelOrigin->x = (globalMuzzlePosition.x - (_loc1_ * direction->x));
	barrelOrigin->y = (globalMuzzlePosition.y - (_loc1_ * direction->y));
	barrelOrigin->z = (globalMuzzlePosition.z - (_loc1_ * direction->z));
}
