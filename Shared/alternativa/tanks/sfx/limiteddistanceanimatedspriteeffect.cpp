#include "limiteddistanceanimatedspriteeffect.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "alternativa/tanks/sfx/Object3DPositionProvider.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/camera/gamecamera.h"



LimitedDistanceAnimatedSpriteEffect::LimitedDistanceAnimatedSpriteEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	 this->sprite = new_AnimatedSprite3D(1, 1);
}

void LimitedDistanceAnimatedSpriteEffect::init(double width,
											   double height,
											   TextureAnimation *textureAnimation,
											   double rotation,
											   Object3DPositionProvider *positionProvider,
											   double originX,
											   double originY,
											   std::shared_ptr<ColorTransform> param8,
											   double softAttenuation,
											   BlendMode blendMode,
											   double fallOffStart,
											   double distanceToDisable,
											   double alphaMultiplier,
											   bool shadowMapLight)
{
	this->alphaMultiplier = alphaMultiplier;
	this->initSprite(width, height, rotation, originX, originY, param8, textureAnimation, softAttenuation, blendMode);
	this->fallOffStart = fallOffStart;
	this->distanceToDisable = distanceToDisable;
	positionProvider->initPosition(this->sprite.get());
	this->framesPerMs = (0.001 * textureAnimation->fps);
	this->positionProvider = positionProvider;
	this->currentFrame = 0;
	this->loopsCount = 1;
	this->sprite->useShadowMap = shadowMapLight;
	this->sprite->useLight = shadowMapLight;
	this->sprite->softAttenuation = 80;
}

void LimitedDistanceAnimatedSpriteEffect::initSprite(double width,
													 double height,
													 double rotation,
													 double originX,
													 double originY,
													 std::shared_ptr<ColorTransform> &colorTransform,
													 TextureAnimation *textureAnimation,
													 double softAttenuation,
													 BlendMode blendMode)
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

ClientObjectUser* LimitedDistanceAnimatedSpriteEffect::getOwner()
{
	return nullptr;
}

void LimitedDistanceAnimatedSpriteEffect::kill()
{
	this->loopsCount = 1;
	this->currentFrame = this->sprite->getNumFrames();
}

void LimitedDistanceAnimatedSpriteEffect::destroy()
{
	this->container->removeChild(this->sprite.get());
	this->container = nullptr;
	//this->sprite->clear();
	this->positionProvider->destroy();
	this->positionProvider = nullptr;
	storeInPool();
}

bool LimitedDistanceAnimatedSpriteEffect::play(int millis, GameCamera *camera)
{
	this->sprite->setFrameIndex(this->currentFrame);
	this->currentFrame += millis * this->framesPerMs;
	this->positionProvider->updateObjectPosition(this->sprite.get(), camera, millis);
	if (((this->loopsCount > 0) && (this->currentFrame >= this->sprite->getNumFrames())))
	{
		this->loopsCount--;
		if (this->loopsCount == 0)
		{
			return false;
		}
		this->currentFrame = (this->currentFrame - this->sprite->getNumFrames());
	}

	Vector3 effectPosition;
	effectPosition.copy(&this->sprite->position);

	double distance = effectPosition.distanceTo(&camera->pos);

	if (distance > this->distanceToDisable)
	{
		this->sprite->visible = false;
	}
	else
	{
		this->sprite->visible = true;
		if (distance > this->fallOffStart)
		{
			this->sprite->alpha = (this->alphaMultiplier * (this->distanceToDisable - distance)) / (this->distanceToDisable - this->fallOffStart);
		}
		else
		{
			this->sprite->alpha = this->alphaMultiplier;
		}
	}

	return true;
}

void LimitedDistanceAnimatedSpriteEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->sprite.get());
}
