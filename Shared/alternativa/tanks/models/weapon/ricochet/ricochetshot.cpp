#include "ricochetshot.h"
#include "mymath.h"
#include "alternativa/math/vector3.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "tailtrail.h"
#include "randomgenerator.h"
#include "alternativa/tanks/models/sfx/shoot/ricochet/RicochetSFXData.h"
#include "RicochetData.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/tanks/models/weapon/weakening/weaponweakeningmodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "IRicochetShotListener.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/sfx/sfxutils.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/engine3d/lights/omnilight.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/battlefield/gamemode/gamemodes.h"
#include "alternativa/engine3d/core/object3dcontainer.h"
#include "__global.h"


static const double SHOT_SIZE = 300;
static const double BUMP_FLASH_SIZE = 80;
static const int NUM_RADIAL_RAYS = 6;
static const double RADIAL_ANGLE_STEP = (2 * MyMath::PI) / NUM_RADIAL_RAYS;//1.0471975511966
static int counter = 0;


RicochetShot::RicochetShot(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	for (int i = 0; i < NUM_RADIAL_RAYS; i++)
		this->radialPoints[i].reset();

	this->sprite = new_AnimatedSprite3D(300, 300);
	this->sprite->softAttenuation = 0;
	this->sprite->looped = true;
#ifdef GRAPHICS
	this->tailTrail = new_TailTrail(100, 300);
	this->omni = new_OmniLight(RandomGenerator::random() * 0xFFFFFF, (SHOT_SIZE / 2) - 50, SHOT_SIZE);
#endif
}

void RicochetShot::init(const Vector3 *startPos,
						const Vector3 *direction,
						TankData *shooterData,
						RicochetData *ricochetData,
						RicochetSFXData *sfxData,
						ICollisionDetector *collisionDetector,
						IRicochetShotListener *listener,
						IBattleField *battlefield)
{
	this->currPos.copy(startPos);
	this->direction.copy(direction);
	this->shooterData = shooterData;
	this->ricochetData = ricochetData;
	this->sfxData = sfxData;
	this->collisionDetector = collisionDetector;
	this->listener = listener;
	this->battleField = (BattlefieldModel*)battlefield;
	this->id = counter++;
#ifdef GRAPHICS
	this->numFrames = sfxData->shotMaterials->length();
	this->currFrame = this->numFrames * RandomGenerator::random();
#else
	this->numFrames = 0;
	this->currFrame = 0;
#endif
	this->sprite->rotation = 6.28 * RandomGenerator::random();
	this->sprite->colorTransform = nullptr;
#ifdef GRAPHICS
	TextureAnimation *animaton = sfxData->dataShot;
	this->sprite->setAnimationData(animaton);
	this->sprite->setFrameIndex((this->sprite->getNumFrames() * RandomGenerator::random()));
	//this->tailTrail->material = sfxData->tailTrailMaterial;
	this->tailTrail->colorTransform = nullptr;
#endif
	this->initRadialPoints(startPos, direction, ricochetData->shotRadius);
	this->totalDistance = 0;
	this->ricochetCount = 0;
	this->isFirstFrame = true;
}

void RicochetShot::addToContainer(Scene3DContainer *container)
{
	container->addChild(this->sprite.get());
#ifdef GRAPHICS
	container->addChild(this->tailTrail.get());

	if (GameModes::isHalloween(this->battleField->gameMode()))
	{
		container->addChild(this->omni.get());
		this->omni->color = RandomGenerator::random() * 0xFFFFFF;
	}
	if (GameModes::isDay(this->battleField->gameMode()))
	{
		container->addChild(this->omni.get());
		this->omni->color = 0xFFFFFF;
	}
#endif
}

ClientObjectUser* RicochetShot::getOwner()
{
	return nullptr;
}

bool RicochetShot::play(int millis, GameCamera *camera)
{
	(void)camera;

	if (this->totalDistance > this->ricochetData->shotDistance)
	{
		return false;
	}

	RayIntersection _rayHit;

	if (this->isFirstFrame == true)
	{
		this->isFirstFrame = false;
		Vector3 _rayOrigin;
		_rayOrigin.x = this->currPos.x;
		_rayOrigin.y = this->currPos.y;
		_rayOrigin.z = this->currPos.z + this->ricochetData->shotRadius;
		Vector3 _rayDirection;
		_rayDirection.x = 0;
		_rayDirection.y = 0;
		_rayDirection.z = -1;



		if (this->collisionDetector->intersectRayWithStatic(&_rayOrigin,
														   &_rayDirection,
														   CollisionGroup::STATIC,
														   this->ricochetData->shotRadius,
														   nullptr,
														   &_rayHit))
		{
			return false;
		}
	}

	Vector3 _normal;
	_normal.reset();


	double dt = millis / 1000.;
	double frameDistance = this->ricochetData->shotSpeed * dt;
	while (frameDistance > 0)
	{
		double hitTime = -1;
		double currDistance;
		if (this->collisionDetector->intersectRay(&this->currPos, &this->direction, CollisionGroup::WEAPON, frameDistance, this, &_rayHit))
		{
			hitTime = _rayHit.t;
			Body *body = _rayHit.primitive->body;
			if (body != nullptr)
			{
				this->currPos.addScaled(hitTime, &this->direction);
				this->listener->shotHit(this, &this->currPos, &this->direction, body);
				return false;
			}
			currDistance = hitTime;
			_normal.copy(&_rayHit.normal);
		}
		else
		{
			currDistance = frameDistance;
		}

		for (int i = 0; i < NUM_RADIAL_RAYS; i++)
		{
			Vector3 *origin = &this->radialPoints[i];
			if (this->collisionDetector->intersectRay(origin, &this->direction, CollisionGroup::WEAPON, currDistance, nullptr, &_rayHit))
			{
				Body *body = _rayHit.primitive->body;
				if (body != nullptr && (!(this->shooterData->tank == body && this->ricochetCount == 0)))
				{
					this->currPos.addScaled(_rayHit.t, &this->direction);
					this->listener->shotHit(this, &this->currPos, &this->direction, body);
					return false;
				}
			}
			origin->addScaled(currDistance, &this->direction);
		}

		if (hitTime > -1)
		{
			this->ricochetCount++;
			this->totalDistance = (this->totalDistance + hitTime);
			frameDistance = frameDistance - hitTime;
			this->currPos.addScaled(hitTime, &this->direction);
			this->currPos.addScaled(0.1, &_normal);
			this->direction.addScaled(-2 * this->direction.dot(&_normal), &_normal);
			this->initRadialPoints(&this->currPos, &this->direction, this->ricochetData->shotRadius);
#ifdef GRAPHICS
			ObjectPoolService *objectPoolService = battleField->getObjectPoolService();
			AnimatedSpriteEffectNew *bumpFlashEffect = objectPoolService->getAnimatedSpriteEffectNew();
			StaticObject3DPositionProvider *posProvider = objectPoolService->getStaticObject3DPositionProvider();
			posProvider->init(&this->currPos, 50);
			TextureAnimation *animaton = this->sfxData->dataFlash;
			bumpFlashEffect->init(BUMP_FLASH_SIZE, BUMP_FLASH_SIZE, animaton, ((RandomGenerator::random() * MyMath::PI) * 2), posProvider, 0.5, 0.5, nullptr, 0);
			this->battleField->addGraphicEffect(bumpFlashEffect);
			//this->addSoundEffect(this->sfxData.ricochetSound, this->currPos, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 0.8);
#endif
		}
		else
		{
			this->totalDistance = (this->totalDistance + frameDistance);
			this->currPos.addScaled(frameDistance, &this->direction);
			frameDistance = 0;
		}
	}

	this->sprite->position.copy(&this->currPos);
#ifdef GRAPHICS
	if (GameModes::isHalloween(this->battleField->gameMode()) || GameModes::isDay(this->battleField->gameMode()))
	{
		this->omni->position.x = this->currPos.x;
	}


	this->omni->position.y = this->currPos.y;
	this->omni->position.z = this->currPos.z;
#endif
	this->sprite->update(dt);
	double impactCoeff = WeaponWeakeningModel::getImpactCoeff(this->shooterData->turret, 0.01 * this->totalDistance);
	double size = SHOT_SIZE * impactCoeff;
	this->sprite->width = size;
	this->sprite->height = size;
	this->sprite->rotation -= 0.003 * millis;
#ifdef GRAPHICS
	const Vector3 *cameraPosition = &camera->pos;
	SFXUtils::alignObjectPlaneToView(this->tailTrail.get(), &this->currPos, &this->direction, &camera->pos);
	double dx = this->currPos.x - cameraPosition->x;
	double dy = this->currPos.y - cameraPosition->y;
	double dz = this->currPos.z - cameraPosition->z;
	double len = (dx * dx) + (dy * dy) + (dz * dz);
	if (len > 1E-5)
	{
		len = 1. / MyMath::sqrt(len);
		dx *= len;
		dy *= len;
		dz *= len;
	}

	double dot = (dx * this->direction.x) + (dy * this->direction.y) + (dz * this->direction.z);
	if (dot < 0)
	{
		dot = -dot;
	}

	if (dot > 0.5)
	{
		this->tailTrail->alpha = ((2 * (1 - dot)) * impactCoeff);
	}
	else
	{
		this->tailTrail->alpha = impactCoeff;
	}

#endif

	return true;
}

void RicochetShot::destroy()
{
#ifdef GRAPHICS
	this->sprite->removeFromParent();

	if (GameModes::isHalloween(this->battleField->gameMode()) || GameModes::isDay(this->battleField->gameMode()))
		this->omni->getParent()->removeChild(this->omni.get());

	this->tailTrail->removeFromParent();
	//this->tailTrail->material = nullptr;
	this->tailTrail->colorTransform = nullptr;
#endif

	this->sprite->material = nullptr;
	this->sprite->colorTransform = nullptr;
	this->shooterData = nullptr;
	this->ricochetData = nullptr;
	this->sfxData = nullptr;
	this->collisionDetector = nullptr;
	this->listener = nullptr;
	this->battleField = nullptr;
	this->storeInPool();
}

void RicochetShot::kill()
{
	this->totalDistance = this->ricochetData->shotDistance + 1;
}

bool RicochetShot::considerBody(Body *body)
{
	return !(body == this->shooterData->tank && this->ricochetCount == 0);
}


void RicochetShot::initRadialPoints(const Vector3 *centerPoint, const Vector3 *dir, double radius)
{
	int axis;
	double min = 1E308;

	double d = (dir->x < 0) ? -dir->x : dir->x;

	if (d < min)
	{
		min = d;
		axis = 0;
	}

	d = (dir->y < 0) ? -dir->y : dir->y;
	if (d < min)
	{
		min = d;
		axis = 1;
	}

	d = (dir->z < 0) ? -dir->z : dir->z;
	if (d < min)
	{
		axis = 2;
	}

	Vector3 v;

	switch (axis)
	{
		case 0:
			v.x = 0;
			v.y = dir->z;
			v.z = -dir->y;
			break;
		case 1:
			v.x = -dir->z;
			v.y = 0;
			v.z = dir->x;
			break;
		case 2:
			v.x = dir->y;
			v.y = -dir->x;
			v.z = 0;
			break;
	}

	v.normalize();
	v.scale(radius);

	Matrix3 rotationMatrix;
	rotationMatrix.fromAxisAngle(dir, RADIAL_ANGLE_STEP);

	Vector3 *v2 = &this->radialPoints[0];
	v2->copy(centerPoint);
	v2->add(&v);

	for (int i = 1; i < NUM_RADIAL_RAYS; i++)
	{
		v.transformBy3(&rotationMatrix);
		Vector3 *v2 = &this->radialPoints[i];
		v2->copy(centerPoint);
		v2->add(&v);
	}
}

void RicochetShot::addSoundEffect(Sound *sound, const Vector3 *position, double nearRadius, double farRadius, double farDelimiter, double soundVolume)
{
	(void)sound;
	(void)position;
	(void)nearRadius;
	(void)farRadius;
	(void)farDelimiter;
	(void)soundVolume;
#if 0
	if (sound != null)
	{
		Sound3D sound3d = Sound3D.create(sound, nearRadius, farRadius, farDelimiter, soundVolume);
		Sound3DEffect soundEffect = Sound3DEffect.create(objectPool, null, position, sound3d);
		this->battlefield.addSound3DEffect(soundEffect);
	}
#endif
}
