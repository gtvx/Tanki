#include "muzzlepositionprovider.h"
#include "alternativa/engine3d/core/object3d.h"


MuzzlePositionProvider::MuzzlePositionProvider(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	localPosition = new Vector3();
}

MuzzlePositionProvider::~MuzzlePositionProvider()
{
	delete localPosition;
}

void MuzzlePositionProvider::init(Object3D *turret, const Vector3 *pos, double offset_y)
{
	this->turret = turret;
	this->localPosition->copy(pos);
	this->localPosition->y += offset_y;
}

void MuzzlePositionProvider::initPosition(Object3D *)
{

}

void MuzzlePositionProvider::updateObjectPosition(Object3D *o, GameCamera *camera, int v)
{
	(void)camera;
	(void)v;

	Vector3 globalPosition;
	Matrix4 turretMatrix;
	turretMatrix.setMatrix(&this->turret->position, &this->turret->rotation);
	turretMatrix.transformVector(this->localPosition, &globalPosition);
	o->position.copy(&globalPosition);
}

void MuzzlePositionProvider::destroy()
{
	this->turret = nullptr;
	this->storeInPool();
}
