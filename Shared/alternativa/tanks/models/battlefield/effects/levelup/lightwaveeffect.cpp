#include "lightwaveeffect.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/engine3d/materials/texturematerial.h"



LightWaveEffect::LightWaveEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->wave = new_Sprite3D(10, 10, nullptr);
	this->wave->blendMode = BlendMode::ADD;
}

void LightWaveEffect::init(int delay, double size, double maxScale, bool direction, Object3D *turret, TextureMaterial *material)
{
	this->delay = delay;
	this->wave->width = size;
	this->wave->height = size;
	this->wave->calculateBounds();
	this->maxScale = maxScale;
	this->direction = direction;
	this->turret = turret;
	material->resolution = 5;
	this->wave->material = material->ptr.lock();
	this->wave->scale.x = 1;
	this->wave->scale.y = 1;
	this->wave->scale.z = 1;
	this->wave->rotation = 0;
	this->wave->alpha = 0;
	this->state = 0;
	this->wave->softAttenuation = 150;
	this->wave->depthMapAlphaThreshold = 2;
	this->wave->shadowMapAlphaThreshold = 2;
	this->wave->useShadowMap = false;
	this->wave->useLight = false;
	this->time = 0;
}

void LightWaveEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->wave.get());
}


bool LightWaveEffect::play(int millis, GameCamera *camera)
{
	if (this->state == 2)
		return false;

	double _loc3_ = 0.7 * 5;
	double _loc4_ = _loc3_ / 3;
	double _loc6_ = 300;
	Vector3 vector;
	vector.x = camera->position.x - this->turret->position.x;
	vector.y = camera->position.y - this->turret->position.y;
	vector.z = camera->position.z - this->turret->position.z;
	vector.normalize();
	vector.scale(_loc6_);
	this->wave->position.x = (this->turret->position.x + vector.x);
	this->wave->position.y = (this->turret->position.y + vector.y);
	this->wave->position.z = ((this->turret->position.z + vector.z) + 30);
	this->time += millis;
	if (this->time >= this->delay)
	{
		if (this->direction)
		{
			this->wave->rotation = (this->wave->rotation + ((0.2 * millis) * 0.001));
		}
		else
		{
			this->wave->rotation = (this->wave->rotation - ((0.2 * millis) * 0.001));
		}
		if (this->state == 0)
		{
			double value = ((_loc3_ * millis) * 0.001);
			this->wave->scale.x = (this->wave->scale.x + value);
			this->wave->scale.y = (this->wave->scale.y + value);
			this->wave->scale.z = (this->wave->scale.z + value);
			if (this->wave->scale.x > this->maxScale)
			{
				this->wave->scale.x = this->maxScale;
				this->wave->scale.y = this->maxScale;
				this->wave->scale.z = this->maxScale;
				this->state = 1;
			}
			this->wave->alpha = (this->wave->scale.x - 1) / (this->maxScale - 1);
		}
		else
		{
			if (this->state == 1)
			{
				double value = ((_loc4_ * millis) * 0.001);
				this->wave->scale.x = (this->wave->scale.x - value);
				this->wave->scale.y = (this->wave->scale.y - value);
				this->wave->scale.z = (this->wave->scale.z - value);
				if (this->wave->scale.x < 1)
				{
					this->wave->scale.x = 1;
					this->wave->scale.y = 1;
					this->wave->scale.z = 1;
					this->state = 2;
				}
				this->wave->alpha = ((this->wave->scale.x - 1) / (this->maxScale - 1)) - 0.1;
			}
		}
	}
	return true;
}

ClientObjectUser* LightWaveEffect::getOwner()
{
	return nullptr;
}

void LightWaveEffect::destroy()
{
	this->container->removeChild(this->wave.get());
	this->container = nullptr;
	this->wave->material = nullptr;
	this->storeInPool();
}

void LightWaveEffect::kill()
{
	this->state = 2;
	this->wave->alpha = 0;
}
