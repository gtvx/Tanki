#include "animatedspriteeffectnew.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "Object3DPositionProvider.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"


AnimatedSpriteEffectNew::AnimatedSpriteEffectNew(ObjectPool *objectPool)
	: PooledObject(objectPool, 6)
{
	this->sprite = new_AnimatedSprite3D(1, 1);
}

void AnimatedSpriteEffectNew::init(double width, double height,
								   TextureAnimation *textureAnimation,
								   double rotation,
								   Object3DPositionProvider *object3DPositionProvider,
								   double originX, double originY,
								   std::shared_ptr<ColorTransform> colorTransform,
								   double softAttenuation,
								   BlendMode blendMode)
{
	this->initSprite(width, height, rotation, originX, originY,
					 colorTransform, textureAnimation, softAttenuation,
					 blendMode);


	object3DPositionProvider->initPosition(this->sprite.get());
	this->framesPerMs = 0.001 * textureAnimation->fps;
	this->positionProvider = object3DPositionProvider;
	this->currentFrame = 0;
	this->loopsCount = 1;
}

void AnimatedSpriteEffectNew::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->sprite.get());
}

bool AnimatedSpriteEffectNew::play(int millis, GameCamera *camera)
{
	this->sprite->setFrameIndex(int(this->currentFrame));
	this->currentFrame += millis * this->framesPerMs;
	this->positionProvider->updateObjectPosition(this->sprite.get(), camera, millis);

	if ((this->loopsCount > 0) && (this->currentFrame >= this->sprite->getNumFrames()))
	{
		this->loopsCount--;
		if (this->loopsCount == 0)
		{
			return false;
		}
		this->currentFrame -= this->sprite->getNumFrames();
	}

	return true;
}

void AnimatedSpriteEffectNew::destroy()
{
	this->container->removeChild(this->sprite.get());
	this->container = nullptr;
	//this->sprite->clear();
	//this->sprite->destroy();
	this->positionProvider->destroy();
	this->positionProvider = nullptr;
	this->storeInPool();
}

void AnimatedSpriteEffectNew::kill()
{
	this->loopsCount = 1;
	this->currentFrame = this->sprite->getNumFrames();
}

void AnimatedSpriteEffectNew::initSprite(double width, double height, double rotation, double originX, double originY, std::shared_ptr<ColorTransform> &colorTransform, TextureAnimation *textureAnimation, double softAttenuation, BlendMode blendMode)
{
	this->sprite->width = width;
	this->sprite->height = height;
	this->sprite->rotation = rotation;
	this->sprite->originX = originX;
	this->sprite->originY = originY;
	this->sprite->blendMode = blendMode;
	this->sprite->colorTransform = colorTransform;
	this->sprite->softAttenuation = softAttenuation;
	this->sprite->setAnimationData(textureAnimation);
}

ClientObjectUser *AnimatedSpriteEffectNew::getOwner()
{
	return nullptr;
}
