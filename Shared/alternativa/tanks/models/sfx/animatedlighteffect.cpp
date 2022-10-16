#include "animatedlighteffect.h"
#include "alternativa/engine3d/lights/omnilight.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/sfx/lightanimation.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/sfx/Object3DPositionProvider.h"
#include "mymath.h"


static double distanceBetween(const Vector3 *a, const Vector3 *b)
{
	double x = a->x - b->x;
	double y = a->y - b->y;
	double z = a->z - b->z;
	return MyMath::sqrt((x * x) + (y * y) + (z * z));
}



AnimatedLightEffect::AnimatedLightEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->light = new_OmniLight(0, 0, 0);
	this->position = new Vector3();
	this->animation = nullptr;
	this->alive = false;
}

void AnimatedLightEffect::init(Object3DPositionProvider *positionProvider, LightAnimation *animation, double maxDistance, bool looped)
{
	this->initFromTime(positionProvider, animation->getLiveTime(), animation, maxDistance, looped);
}

void AnimatedLightEffect::initFromTime(Object3DPositionProvider *positionProvider, int timeToLive, LightAnimation *animation, double maxDistance, bool looped)
{
	this->positionProvider = positionProvider;
	this->timeToLive = timeToLive;
	this->currentTime = 0;
	this->animation = animation;
	this->looped = looped;
	this->alive = true;
	this->maxDistance = maxDistance;
	this->fadeDistance = (maxDistance / 4) * 3;
}

void AnimatedLightEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->light.get());
}

bool AnimatedLightEffect::play(int millis, GameCamera *camera)
{
	if (this->alive)
	{
		this->animation->updateByTime(this->light.get(), this->currentTime, this->timeToLive);
		this->positionProvider->updateObjectPosition(this->light.get(), camera, millis);
		this->currentTime = (this->currentTime + millis);
		if (this->currentTime > this->timeToLive)
		{
			if (this->looped)
			{
				this->currentTime = (this->currentTime % this->timeToLive);
			}
			else
			{
				this->alive = false;
			}
		}
		this->position->copy(&this->light->position);
		double _loc3_ = distanceBetween(this->position, &camera->pos);
		if (_loc3_ > this->fadeDistance)
		{
			double _loc4_ = (1 - ((_loc3_ - this->fadeDistance) / (this->maxDistance - this->fadeDistance)));
			this->light->intensity = (this->light->intensity * _loc4_);
			this->light->visible = (_loc3_ < this->maxDistance);
		}
		return this->alive;
	}
	return false;
}

void AnimatedLightEffect::destroy()
{
	this->container->removeChild(this->light.get());
	this->container = nullptr;
	this->animation = nullptr;
	this->positionProvider->destroy();
	this->positionProvider = nullptr;
	this->storeInPool();
}

void AnimatedLightEffect::kill()
{
	this->alive = false;
}

ClientObjectUser* AnimatedLightEffect::getOwner()
{
	return nullptr;
}
