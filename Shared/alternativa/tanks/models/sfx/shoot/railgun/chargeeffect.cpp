#include "chargeeffect.h"
#include "alternativa/engine3d/objects/animsprite.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"


ChargeEffect::ChargeEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	localMuzzlePosition = new Vector3();
}

void ChargeEffect::init(ClientObjectUser *owner,
						double width,
						double height,
						QVector<std::shared_ptr<Material>> *materials,
						const Vector3 *localMuzzlePosition,
						Object3D *turret,
						double rotation,
						double fps,
						std::shared_ptr<ColorTransform> &colorTransform)
{
	this->_owner = owner;
	this->initSprite(width, height, rotation, colorTransform);
	this->materials = materials;
	this->framesPerMillisecond = (0.001 * fps);
	this->localMuzzlePosition->copy(localMuzzlePosition);
	this->localMuzzlePosition->y = this->localMuzzlePosition->y + 10;
	this->turret = turret;
	this->numFrames = materials->length();
	this->currFrame = 1;
	this->sprite->setMaterials(materials);
	this->sprite->material = materials->at(1);
	this->sprite->setFrame(1);
	this->sprite->softAttenuation = 140;
	this->sprite->setLoop(true);
}

void ChargeEffect::addToContainer(Scene3DContainer *container)
{
	container->addChild(this->sprite.get());
}

ClientObjectUser *ChargeEffect::getOwner()
{
	return this->_owner;
}

bool ChargeEffect::play(int millis, GameCamera *camera)
{
	(void)camera;

	if ((this->currFrame + 1) >= this->numFrames)
		return false;

	Vector3 globalPosition;
	Matrix4 matrix;
	matrix.setMatrix(&this->turret->position, &this->turret->rotation);
	matrix.transformVector(this->localMuzzlePosition, &globalPosition);
	this->sprite->position.copy(&globalPosition);
	this->sprite->setFrame(int(this->currFrame));
	this->currFrame += this->framesPerMillisecond * millis;
	return true;
}

void ChargeEffect::destroy()
{
	this->sprite->removeFromParent();
	//this->sprite->material = nullptr;
	//this->sprite->destroy();
	this->materials = nullptr;
	this->storeInPool();
}

void ChargeEffect::kill()
{
	this->currFrame = (this->numFrames + 1);
}

void ChargeEffect::initSprite(double width, double height, double rotation, std::shared_ptr<ColorTransform> &colorTransform)
{
	if (this->sprite == nullptr)
	{
		this->sprite = new_AnimSprite(width, height);
	}
	else
	{
		this->sprite->width = width;
		this->sprite->height = height;
	}
	this->sprite->rotation = rotation;
	this->sprite->colorTransform = colorTransform;
}
