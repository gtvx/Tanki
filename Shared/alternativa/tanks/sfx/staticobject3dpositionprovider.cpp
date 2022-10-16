#include "staticobject3dpositionprovider.h"
#include "alternativa/tanks/camera/gamecamera.h"


StaticObject3DPositionProvider::StaticObject3DPositionProvider(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{

}

void StaticObject3DPositionProvider::init(const Vector3 *position, double offsetToCamera)
{
	this->position.copy(position);
	this->offsetToCamera = offsetToCamera;
}

void StaticObject3DPositionProvider::initPosition(Object3D *object)
{
	object->position.x = this->position.x;
	object->position.y = this->position.y;
	object->position.z = this->position.z;
}

void StaticObject3DPositionProvider::updateObjectPosition(Object3D *object, GameCamera *gameCamera, int)
{
	Vector3 toCamera;
	toCamera.x = gameCamera->position.x - this->position.x;
	toCamera.y = gameCamera->position.y - this->position.y;
	toCamera.z = gameCamera->position.z - this->position.z;
	toCamera.normalize();
	object->position.x = this->position.x + (this->offsetToCamera * toCamera.x);
	object->position.y = this->position.y + (this->offsetToCamera * toCamera.y);
	object->position.z = this->position.z + (this->offsetToCamera * toCamera.z);
}

void StaticObject3DPositionProvider::destroy()
{
	this->storeInPool();
}
