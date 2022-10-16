#include "scalingobject3dpositionprovider.h"
#include "alternativa/engine3d/core/object3d.h"


ScalingObject3DPositionProvider::ScalingObject3DPositionProvider(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	initialPosition = new Vector3();
	velocity = new Vector3();
}

ScalingObject3DPositionProvider::~ScalingObject3DPositionProvider()
{
	delete initialPosition;
	delete velocity;
}

void ScalingObject3DPositionProvider::init(const Vector3 *pos, const Vector3 *velocity, double scaleVelocity)
{
	this->initialPosition->copy(pos);
	this->velocity->copy(velocity);
	this->scaleVelocity = scaleVelocity;
}

void ScalingObject3DPositionProvider::initPosition(Object3D *object)
{
	object->position.copy(this->initialPosition);
	object->scale.reset(1, 1, 1);
}

void ScalingObject3DPositionProvider::updateObjectPosition(Object3D *object, GameCamera *camera, int param3)
{
	(void)camera;
	double v = 0.001 * param3;
	object->position.x += this->velocity->x * v;
	object->position.y += this->velocity->y * v;
	object->position.z += this->velocity->z * v;
	object->scale.x +=  this->scaleVelocity;
	object->scale.y += this->scaleVelocity;
	object->scale.z += this->scaleVelocity;
}

void ScalingObject3DPositionProvider::destroy()
{
	storeInPool();
}
