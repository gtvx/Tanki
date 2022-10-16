#include "movingobject3dpositionprovider.h"
#include "alternativa/engine3d/core/object3d.h"


MovingObject3DPositionProvider::MovingObject3DPositionProvider(ObjectPool *objectPool)
	: PooledObject(objectPool, 7)
{

}

void MovingObject3DPositionProvider::initPosition(Object3D *o)
{
	o->position.copy(&this->initialPosition);
}

void MovingObject3DPositionProvider::init(const Vector3 *initialPosition, const Vector3 *velocity, double acceleration)
{
	this->initialPosition.copy(initialPosition);
	this->velocity.copy(velocity);
	this->acceleration = acceleration;
}

void MovingObject3DPositionProvider::updateObjectPosition(Object3D *o, GameCamera *camera, int param3)
{
	(void)camera;

	double _loc4_ = 0.001 * param3;
	o->position.x = (o->position.x + (this->velocity.x * _loc4_));
	o->position.y = (o->position.y + (this->velocity.y * _loc4_));
	o->position.z = (o->position.z + (this->velocity.z * _loc4_));
	double _loc5_ = this->velocity.length();
	_loc5_ = _loc5_ + (this->acceleration * _loc4_);
	if (_loc5_ <= 0)
	{
		this->velocity.reset();
	}
	else
	{
		this->velocity.normalize();
		this->velocity.scale(_loc5_);
	}
}

void MovingObject3DPositionProvider::destroy()
{
	this->storeInPool();
}
