#include "streamlighteffect.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "randomgenerator.h"
#include "alternativa/engine3d/core/light3d.h"
#include "lightanimation.h"
#include "alternativa/tanks/sfx/Object3DPositionProvider.h"


static const int FADE_TIME = 250;

StreamLightEffect::StreamLightEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{

}

void StreamLightEffect::init(Light3D *light)
{
	this->light = light;
}

void StreamLightEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->light);
}

void StreamLightEffect::startLoop()
{
	this->currentLoopTime = (this->loopTime + ((RandomGenerator::random() * this->random) - (this->random / 2.)));
}


bool StreamLightEffect::play(int millis, GameCamera *camera)
{
	if (!this->alive)
	{
		return false;
	}

	if (this->starting)
	{
		this->currentTime += millis;
		this->startAnimation->updateByTime(this->light, this->currentTime, this->startTime);
		if (this->currentTime >= this->startTime)
		{
			this->starting = false;
			this->currentTime = 0;
			this->startLoop();
		}
	}
	else
	{
		this->currentTime += millis;
		if (this->currentTime > this->currentLoopTime)
		{
			this->currentTime = this->currentTime % this->currentLoopTime;
			this->startLoop();
		}
		this->loopAnimation->updateByTime(this->light, this->currentTime, this->loopTime);
	}

	this->positionProvider->updateObjectPosition(this->light, camera, millis);

	if (this->fading)
	{
		this->fadeTime += millis;
		if (this->fadeTime <= FADE_TIME)
		{
			this->light->intensity = (this->light->intensity * (1 - (this->fadeTime / FADE_TIME)));
		}
		else
		{
			this->light->intensity = 0;
			this->kill();
		}
	}
	return true;
}

void StreamLightEffect::destroy()
{
	this->container->removeChild(this->light);
	this->container = nullptr;
	this->startAnimation = nullptr;
	this->loopAnimation = nullptr;
	this->positionProvider->destroy();
	this->positionProvider = nullptr;
	this->storeInPool();
}

void StreamLightEffect::kill()
{
	this->alive = false;
}

void StreamLightEffect::stop()
{
	this->fading = true;
	this->fadeTime = 0;
}

ClientObjectUser *StreamLightEffect::getOwner()
{
	return nullptr;
}
