#include "animatedplaneeffectold.h"
#include "alternativa/engine3d/primitives/plane.h"
#include "alternativa/engine3d/core/Clipping.h"
#include "alternativa/engine3d/core/Sorting.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"


static const double BASE_SIZE = 100;


AnimatedPlaneEffectOld::AnimatedPlaneEffectOld(ObjectPool *objectPool)
	: PooledObject(objectPool, 1)
{
	this->plane = nullptr;
}

void AnimatedPlaneEffectOld::init(double size, const Vector3 *position, const Vector3 *rotation, double fps, QVector<std::shared_ptr<Material>> *frames, double scaleSpeed)
{
	this->framesPerMillisecond = 0.001 * fps;
	this->frames = frames;
	this->scaleSpeed = 0.001 * scaleSpeed;
	this->numFrames = frames->length();
	if (this->plane == nullptr)
	{
		this->plane = new_Plane(BASE_SIZE, BASE_SIZE);
		this->plane->clipping = Clipping::FACE_CLIPPING;
		this->plane->sorting = Sorting::DYNAMIC_BSP;
	}
	this->baseScale = size / BASE_SIZE;
	this->scale = this->baseScale;
	this->plane->position.x = position->x;
	this->plane->position.y = position->y;
	this->plane->position.z = position->z;
	this->plane->rotation.x = rotation->x;
	this->plane->rotation.y = rotation->y;
	this->plane->rotation.z = rotation->z;
	this->currentFrame = 0;
}

void AnimatedPlaneEffectOld::addToContainer(Scene3DContainer *container)
{
	container->addChild(this->plane.get());
}

bool AnimatedPlaneEffectOld::play(int millis, GameCamera *camera)
{
	(void)camera;
	if (this->currentFrame >= this->numFrames)
	{
		return false;
	}
	this->plane->setMaterialToAllFaces(this->frames->at(int(this->currentFrame)).get());
	this->currentFrame = this->currentFrame + (this->framesPerMillisecond * millis);
	this->plane->scale.x = this->scale;
	this->plane->scale.y = this->scale;
	this->scale = this->scale + ((this->baseScale * this->scaleSpeed) * millis);
	return true;
}

void AnimatedPlaneEffectOld::destroy()
{
	this->plane->removeFromParent();
	//this->plane->destroy();
	this->storeInPool();
}

void AnimatedPlaneEffectOld::kill()
{
	this->currentFrame = this->frames->length();
}

ClientObjectUser *AnimatedPlaneEffectOld::getOwner()
{
	return nullptr;
}
