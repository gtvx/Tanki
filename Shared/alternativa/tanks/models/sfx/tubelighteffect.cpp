#include "tubelighteffect.h"
#include "alternativa/engine3d/lights/tubelight.h"
#include "alternativa/tanks/sfx/Object3DPositionProvider.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/models/sfx/lightanimation.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"


static const double CAMERA_DISTANCE_COEF = 1.5;
//private static const position:Vector3 = new Vector3();
//private static const targetPositon:Vector3 = new Vector3();



TubeLightEffect::TubeLightEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->light = new_TubeLight(0, 0, 0, 0, 0);
	this->target = new_Object3D();
}


void TubeLightEffect::init(Object3DPositionProvider *positionProvider, Object3DPositionProvider *targetProvider, LightAnimation *animation, bool looped)
{
	this->initFromTime(positionProvider, targetProvider, animation->getLiveTime(), animation, looped);
}

void TubeLightEffect::initFromTime(Object3DPositionProvider *positionProvider,
								   Object3DPositionProvider *targetProvider,
								   int timeToLive,
								   LightAnimation *animation,
								   bool looped)
{
	this->positionProvider = positionProvider;
	this->targetProvider = targetProvider;
	this->timeToLive = timeToLive;
	this->currentTime = 0;
	this->animation = animation;
	this->looped = looped;
	this->alive = true;
}

void TubeLightEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->light.get());
	container->addChild(this->target.get());
}


bool TubeLightEffect::play(int millis, GameCamera *camera)
{
	if (this->alive == true)
	{
		this->animation->updateByTime(this->light.get(), this->currentTime, this->timeToLive);
		this->positionProvider->updateObjectPosition(this->light.get(), camera, millis);
		this->targetProvider->updateObjectPosition(this->target.get(), camera, millis);
		this->currentTime += millis;
		Vector3 position;
		position.x = this->light->position.x;
		position.y = this->light->position.y;
		position.z = this->light->position.z;
		Vector3 targetPositon;
		targetPositon.x = this->target->position.x;
		targetPositon.y = this->target->position.y;
		targetPositon.z = this->target->position.z;
		double _loc3_ = Vector3::distanceBetween(&position, &targetPositon);
		double _loc4_ = (camera->farClipping / CAMERA_DISTANCE_COEF);
		_loc3_ = ((_loc3_ > _loc4_) ? double(double(_loc4_)) : double(double(_loc3_)));
		this->light->length = _loc3_;
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
		this->light->lookAt(this->target->position.x, this->target->position.y, this->target->position.z);
		this->light->falloff = (this->light->attenuationEnd - this->light->attenuationBegin);
		return this->alive;
	}
	return false;
}

void TubeLightEffect::destroy()
{
	this->container->removeChild(this->light.get());
	this->container->removeChild(this->target.get());
	this->container = nullptr;
	this->animation = nullptr;
	this->positionProvider = nullptr;
	this->storeInPool();
}

void TubeLightEffect::kill()
{
	this->alive = false;
}

ClientObjectUser *TubeLightEffect::getOwner()
{
	return nullptr;
}
