#include "traileffect1.h"
#include "trail1.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/sfx/sfxutils.h"


const double TrailEffect1::WIDTH = 80;

TrailEffect1::TrailEffect1(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->position = new Vector3();
	this->direction = new Vector3();

	this->beam = new_Trail1();
}

void TrailEffect1::init(const Vector3 *position, const Vector3 *direction, double param3, double param4, std::shared_ptr<Material> &material, int timeToLive)
{
	this->position->copy(position);
	this->direction->copy(direction);
	this->timeToLive = timeToLive;
	this->alphaSpeed = 1. / timeToLive;
	this->beam->init(WIDTH, param3, param4, material);
}

bool TrailEffect1::play(int millis, GameCamera *camera)
{
	if (this->timeToLive < 0)
		return false;

	this->timeToLive -= millis;
	this->beam->alpha -= this->alphaSpeed * millis;
	SFXUtils::alignObjectPlaneToView(this->beam.get(), this->position, this->direction, &camera->pos);
	return true;
}

void TrailEffect1::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->beam.get());
}

void TrailEffect1::destroy()
{
	this->container->removeChild(this->beam.get());
	this->container = nullptr;
	this->storeInPool();
}

void TrailEffect1::kill()
{
	this->timeToLive = -1;
}

ClientObjectUser *TrailEffect1::getOwner()
{
	return nullptr;
}
