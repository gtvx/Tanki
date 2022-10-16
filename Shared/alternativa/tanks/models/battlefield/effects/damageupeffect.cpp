#include "damageupeffect.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/engine3d/materials/texturematerial.h"


static const double POP_HEIGHT = 100;
static const double REST_HEIGHT = 250;
static const double MAX_HEIGHT = 300;
static const double POP_SPEED = 1000;
static const double REST_SPEED = 100;

DamageUpEffect::DamageUpEffect(ObjectPool *objectPool)
	: PooledObject(objectPool, 5)
{
	this->damage = new_Sprite3D(50, 30, nullptr);
}

void DamageUpEffect::init(int delay,
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
						  std::shared_ptr<TextureMaterial> material,
						  BlendMode blendMode)
{
	this->delay = delay;
	this->damage->width = width;
	this->damage->height = height;
	this->damage->calculateBounds();
	this->damage->rotation = rotation;
	this->maxHeight = maxHeight;
	this->visibleHeight = visibleHeight;
	this->heightSpeed = heightSpeed;
	this->x = x;
	this->y = y;
	this->z = z;
	this->object = object;
	this->damage->material = material;
	this->damage->softAttenuation = 150;
	this->damage->depthMapAlphaThreshold = 2;
	this->damage->shadowMapAlphaThreshold = 2;
	this->damage->useShadowMap = false;
	this->damage->useLight = false;
	this->damage->depthTest = false;
	this->damage->sorting = 1;
	this->damage->perspectiveScale = false;
	this->damage->blendMode = blendMode;
	this->damage->alpha = 1;
	this->time = 0;
}


void DamageUpEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->damage.get());
}

bool DamageUpEffect::play(int millis, GameCamera *camera)
{
	(void)camera;

	if (this->z >= this->maxHeight)
	{
		return false;
	}

	this->damage->position.x = (this->object->position.x + this->x);
	this->damage->position.y = (this->object->position.y + this->y);
	this->damage->position.z = (this->object->position.z + this->z);
	this->time = this->time + millis;
	this->z = this->z + (((this->maxHeight * this->heightSpeed) * millis) * 0.001);

	if (this->z < this->visibleHeight)
	{
		this->damage->alpha = this->z / this->visibleHeight;
	}
	else
	{
		double v = (this->z - this->visibleHeight) / (this->maxHeight - this->visibleHeight);
		this->damage->alpha = 1. - (((((((v * v) * v) * v) * v) * v) * v) * v);
	}

	if (this->damage->alpha < 0)
		this->damage->alpha = 0;

	if (this->damage->alpha > 1)
		this->damage->alpha = 1;

	return true;
}

ClientObjectUser* DamageUpEffect::getOwner()
{
	return nullptr;
}

void DamageUpEffect::destroy()
{
	this->container->removeChild(this->damage.get());
	this->container = nullptr;
	this->damage->material = nullptr;
	this->storeInPool();
}

void DamageUpEffect::kill()
{
	this->z = this->maxHeight;
	this->damage->alpha = 0;
}
