#include "traileffect2.h"
#include "trail2.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/sfx/sfxutils.h"


const double TrailEffect2::BASE_WIDTH = 48;


TrailEffect2::TrailEffect2(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->position = new Vector3();
	this->direction = new Vector3();

	this->beam = new_Trail2();
}

void TrailEffect2::init(const Vector3 *position,
						const Vector3 *direction,
						double param3,
						double param4,
						std::shared_ptr<Material> &material,
						int timeToLive)
{
	this->position->copy(position);
	this->direction->copy(direction);
	this->timeToLive = timeToLive;
	this->alphaSpeed = 1. / timeToLive;
	this->beam->init(BASE_WIDTH, param3, param4, material);
}

bool TrailEffect2::play(int millis, GameCamera *camera)
{
	if (this->timeToLive < 0)
		return false;

	this->timeToLive -= millis;
	this->beam->alpha -= this->alphaSpeed * millis;
	SFXUtils::alignObjectPlaneToView(this->beam.get(), this->position, this->direction, &camera->pos);
	return true;
}

void TrailEffect2::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->beam.get());
}

void TrailEffect2::destroy()
{
	this->container->removeChild(this->beam.get());
	this->container = nullptr;
	this->storeInPool();
}

void TrailEffect2::kill()
{
	this->timeToLive = -1;
}

ClientObjectUser *TrailEffect2::getOwner()
{
	return nullptr;
}
