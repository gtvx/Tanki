#include "animatedspriteeffect.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/camera/gamecamera.h"


AnimatedSpriteEffect::AnimatedSpriteEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 11)
{
	this->sprite == nullptr;
}

void AnimatedSpriteEffect::init(double width,
								double height,
								QVector<std::shared_ptr<Material>> *materials,
								const Vector3 *position,
								double rotation,
								double offsetToCamera,
								double fps,
								bool loop,
								double originX,
								double originY,
								std::shared_ptr<ColorTransform> colorTransform)
{
	this->initSprite(width, height, rotation, originX, originY, colorTransform);
	this->materials = materials;
	this->offsetToCamera = offsetToCamera;
	this->framesPerMillisecond = 0.001 * fps;
	this->position.copy(position);
	this->loop = loop;
	this->numFrames = materials->length();
	this->sprite->softAttenuation = 140;
	this->currFrame = 0;
}

void AnimatedSpriteEffect::addToContainer(Scene3DContainer *container)
{
	container->addChild(this->sprite.get());
}

ClientObjectUser *AnimatedSpriteEffect::getOwner()
{
	return nullptr;
}

bool AnimatedSpriteEffect::play(int millis, GameCamera *camera)
{
	if (((!(this->loop)) && (this->currFrame >= this->numFrames)))
	{
		return (false);
	}
	Vector3 toCamera;
	toCamera.x = camera->position.x - this->position.x;
	toCamera.y = camera->position.y - this->position.y;
	toCamera.z = camera->position.z - this->position.z;
	toCamera.normalize();
	this->sprite->position.x = this->position.x + (this->offsetToCamera * toCamera.x);
	this->sprite->position.y = this->position.y + (this->offsetToCamera * toCamera.y);
	this->sprite->position.z = this->position.z + (this->offsetToCamera * toCamera.z);
	this->sprite->material = this->materials->at(int(this->currFrame));
	this->currFrame += this->framesPerMillisecond * millis;
	if (this->loop)
	{
		while (this->currFrame >= this->numFrames)
		{
			this->currFrame -= this->numFrames;
		}
	}
	return true;
}

void AnimatedSpriteEffect::destroy()
{
	this->sprite->removeFromParent();
	this->sprite->material = nullptr;
	//this->sprite->destroy();
	//this->sprite = nullptr;
	this->materials = nullptr;
	this->storeInPool();
}

void AnimatedSpriteEffect::kill()
{
	this->loop = false;
	this->currFrame = this->numFrames + 1;
}

void AnimatedSpriteEffect::initSprite(double width,
									  double height,
									  double rotation,
									  double originX,
									  double originY,
									  std::shared_ptr<ColorTransform> colorTransform)
{
	if (this->sprite == nullptr)
	{
		this->sprite = new_Sprite3D(width, height, nullptr);
	}
	else
	{
		this->sprite->width = width;
		this->sprite->height = height;
	};
	this->sprite->rotation = rotation;
	this->sprite->originX = originX;
	this->sprite->originY = originY;
	this->sprite->colorTransform = colorTransform;
}
