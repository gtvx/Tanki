#include "sparkeffect.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/engine3d/materials/texturematerial.h"


SparkEffect::SparkEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->spark = new_Sprite3D(10, 10, nullptr);
}

void SparkEffect::init(int delay,
					   double width,
					   double height,
					   double rotation,
					   double maxHeight,
					   double visibleHeight,
					   double heightSpeed,
					   double x,
					   double y,
					   double z,
					   Object3D *object,
					   TextureMaterial *material,
					   BlendMode blendMode)
{
	this->delay = delay;
	this->spark->width = width;
	this->spark->height = height;
	this->spark->calculateBounds();
	this->spark->rotation = rotation;
	this->maxHeight = maxHeight;
	this->visibleHeight = visibleHeight;
	this->heightSpeed = heightSpeed;
	this->x = x;
	this->y = y;
	this->z = z;
	this->object = object;
	this->spark->material = material->ptr.lock();
	this->spark->softAttenuation = 150;
	this->spark->depthMapAlphaThreshold = 2;
	this->spark->shadowMapAlphaThreshold = 2;
	this->spark->useShadowMap = false;
	this->spark->useLight = false;
	this->spark->blendMode = blendMode;
	this->spark->alpha = 0;
	this->time = 0;
}

void SparkEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->spark.get());
}

bool SparkEffect::play(int millis, GameCamera *camera)
{
	(void)camera;

	if (this->z >= this->maxHeight)
		return false;

	this->spark->position.x = this->object->position.x + this->x;
	this->spark->position.y = this->object->position.y + this->y;
	this->spark->position.z = this->object->position.z + this->z;
	this->time += millis;
	if (this->time >= this->delay)
	{
		this->z = (this->z + (((this->maxHeight * this->heightSpeed) * millis) * 0.001));
		if (this->z < this->visibleHeight)
		{
			this->spark->alpha = (this->z / this->visibleHeight);
		}
		else
		{
			double v = ((this->z - this->visibleHeight) / (this->maxHeight - this->visibleHeight));
			this->spark->alpha = (1 - (((((((v * v) * v) * v) * v) * v) * v) * v));
		}
		if (this->spark->alpha < 0)
		{
			this->spark->alpha = 0;
		}
		if (this->spark->alpha > 1)
		{
			this->spark->alpha = 1;
		}
	}
	else
	{
		this->spark->alpha = 0;
	}
	return true;
}

ClientObjectUser* SparkEffect::getOwner()
{
	return nullptr;
}

void SparkEffect::destroy()
{
	this->container->removeChild(this->spark.get());
	this->container = nullptr;
	this->spark->material = nullptr;
	this->storeInPool();
}

void SparkEffect::kill()
{
	this->z = this->maxHeight;
	this->spark->alpha = 0;
}
