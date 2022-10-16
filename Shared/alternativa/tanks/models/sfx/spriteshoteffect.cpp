#include "spriteshoteffect.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "randomgenerator.h"
#include "mymath.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/engine3d/materials/material.h"


static const int STATE_BRIGHT = 1;
static const int STATE_FADE = 2;


SpriteShotEffect::SpriteShotEffect(ObjectPool *objectPool)
	: PooledObject(objectPool, 2)
{
	this->sprite = new_Sprite3D(100, 100, nullptr);
}

void SpriteShotEffect::init(Material *material,
							const Vector3 *localMuzzlePosition,
							Object3D *turret,
							double localOffset,
							double size,
							int brightTime,
							int fadeTime,
							std::shared_ptr<ColorTransform> &colorTransform)
{
	this->localPosition.copy(localMuzzlePosition);
	this->localPosition.y += localOffset;
	this->turret = turret;
	this->brightTime = brightTime;
	this->fadeTime = fadeTime;
	this->sprite->material = material->ptr.lock();
	this->sprite->width = size;
	this->sprite->height = size;
	this->sprite->rotation = (RandomGenerator::random() * MyMath::PI) * 2;
	this->sprite->colorTransform = colorTransform;
	this->sprite->alpha = 1;
	this->state = STATE_BRIGHT;
	this->time = brightTime;
}

ClientObjectUser *SpriteShotEffect::getOwner()
{
	return nullptr;
}

bool SpriteShotEffect::play(int millis, GameCamera *camera)
{
	(void)camera;

	switch (this->state)
	{
		case STATE_BRIGHT:
			if (this->time < 0)
			{
				this->state = STATE_FADE;
				this->time = this->fadeTime;
			}
			else
			{
				this->time -= millis;
			}
			break;
		case STATE_FADE:
			if (this->time < 0)
			{
				return false;
			}
			this->time -= millis;
			this->sprite->alpha = double(this->time) / double(this->fadeTime);
			break;
	}

	Matrix4 turretMatrix;
	turretMatrix.setMatrix(&this->turret->position, &this->turret->rotation);
	turretMatrix.transformVector(&this->localPosition, &this->sprite->position);
	return true;
}

void SpriteShotEffect::destroy()
{
	this->sprite->removeFromParent();
	this->sprite->material = nullptr;
	this->sprite->colorTransform = nullptr;
	this->turret = nullptr;
	this->storeInPool();
}

void SpriteShotEffect::addToContainer(Scene3DContainer *container)
{
	container->addChild(this->sprite.get());
}

void SpriteShotEffect::kill()
{

}
