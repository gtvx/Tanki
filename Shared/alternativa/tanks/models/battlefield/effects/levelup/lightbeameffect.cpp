#include "lightbeameffect.h"
#include "lightbeam.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"



static const double BASE_SIZE = 300;


LightBeamEffect::LightBeamEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->beam = new_LightBeam(BASE_SIZE);
	this->beam->blendMode = BlendMode::ADD;
}

void LightBeamEffect::init(int delay,
						   double param2,
						   double height,
						   double maxHeight,
						   double heightSpeed,
						   double fadeSpeed,
						   double x,
						   double y,
						   double z,
						   Object3D *turret,
						   TextureMaterial *material)
{
	this->delay = delay;
	this->beam->scale.x = param2 / BASE_SIZE;
	this->height = height;
	this->maxHeight = maxHeight;
	this->heightSpeed = heightSpeed;
	this->fadeSpeed = fadeSpeed;
	this->x = x;
	this->y = y;
	this->z = z;
	this->turret = turret;
	this->beam->init(material);
	this->beam->softAttenuation = 150;
	this->beam->depthMapAlphaThreshold = 2;
	this->beam->shadowMapAlphaThreshold = 2;
	this->beam->useShadowMap = false;
	this->beam->useLight = false;
	this->beam->alpha = 0;
	this->time = 0;
}

void LightBeamEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->beam.get());
}

bool LightBeamEffect::play(int millis, GameCamera *camera)
{
	if ((this->height >= this->maxHeight) && (this->beam->alpha <= 0))
		return false;

	this->beam->position.x = this->turret->position.x + this->x;
	this->beam->position.y = this->turret->position.y + this->y;
	this->beam->position.z = this->turret->position.z + this->z;
	this->beam->rotation.z = camera->rotation.z;
	this->time += millis;

	if (this->time >= this->delay)
	{
		if (this->height < this->maxHeight)
		{
			this->height = this->height + (((this->maxHeight * this->heightSpeed) * millis) * 0.001);
			if (this->height >= this->maxHeight)
			{
				this->height = this->maxHeight;
			}
			this->beam->scale.z = this->height / BASE_SIZE;
			this->beam->alpha = this->height / this->maxHeight;
		}
		else
		{
			this->beam->alpha = this->beam->alpha - ((this->fadeSpeed * millis) * 0.001);
			if (this->beam->alpha < 0)
			{
				this->beam->alpha = 0;
			}
		}
	}

	return true;
}

ClientObjectUser* LightBeamEffect::getOwner()
{
	return nullptr;
}

void LightBeamEffect::destroy()
{
	this->container->removeChild(this->beam.get());
	this->container = nullptr;
	this->turret = nullptr;
	this->beam->clear();
	this->storeInPool();
}

void LightBeamEffect::kill()
{
	this->height = this->maxHeight;
	this->beam->alpha = 0;
}
