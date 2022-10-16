#include "plasmashot.h"
#include "mymath.h"
#include "alternativa/engine3d/lights/omnilight.h"
#include "randomgenerator.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "alternativa/tanks/models/sfx/shoot/plasma/plasmasfxdata.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/weapon/plasma/PlasmaGunData.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "IPlasmaShotListener.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/weapon/weakening/weaponweakeningmodel.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "alternativa/engine3d/core/object3dcontainer.h"
#include "__global.h"


const double PlasmaShot::SIZE = 250;
static const int NUM_RADIAL_RAYS = 8;
static const double RADIAL_ANGLE_STEP = ((2 * MyMath::PI) / NUM_RADIAL_RAYS);//0.785398163397448




PlasmaShot::PlasmaShot(ObjectPool *objectPool)
	: PooledObject(objectPool, 4)
{
	this->shooterData = nullptr;
#ifdef GRAPHICS
	omni = new_OmniLight(RandomGenerator::random() * 0xFFFFFF, (SIZE * 1.1) - 50, SIZE * 2);
	this->omni->intensity = 3;
#endif
	totalDistance = 0;

	this->radialPoints.resize(NUM_RADIAL_RAYS);
	//this->radialPoints = new Vector.<Vector3>(NUM_RADIAL_RAYS);
	for (int i = 0; i < NUM_RADIAL_RAYS; i++)
		this->radialPoints[i] = new Vector3();

	this->sprite = new_AnimatedSprite3D(SIZE, SIZE);
}



void PlasmaShot::init(int shotId,
					  bool active,
					  PlasmaGunData *plasmaData,
					  const Vector3 *startPosition,
					  const Vector3 *direction,
					  TankData *shooterData,
					  IPlasmaShotListener *listener,
					  PlasmaSFXData *sfxData,
					  ICollisionDetector *collisionDetector,
					  IWeaponWeakeningModel *weakeningModel)
{
	this->shotId = shotId;
	this->active = active;
	this->plasmaData = plasmaData;
	this->currPosition.copy(startPosition);
	this->direction.copy(direction);
	this->shooterData = shooterData;
	this->listener = listener;
	this->collisionDetector = collisionDetector;
	this->weakeningModel = weakeningModel;
	this->sfxData = sfxData;
#ifdef GRAPHICS
	TextureAnimation *animationData = sfxData->shotData;
	this->sprite->setAnimationData(animationData);
	this->sprite->setFrameIndex(this->sprite->getNumFrames() * RandomGenerator::random());
	this->sprite->colorTransform = sfxData->colorTransform;
	this->sprite->rotation = 6.28 * RandomGenerator::random();
#endif
	this->sprite->looped = true;
	this->totalDistance = 0;
	this->initRadialPoints(startPosition, direction, plasmaData->shotRadius);
}

void PlasmaShot::addToContainer(Scene3DContainer *container)
{
#ifdef GRAPHICS
	container->addChild(this->sprite.get());
	/*
	if (this->battleField->gameMode == GameModes.HALLOWEEN)
	{
		container->addChild(this->omni);
	}
	*/

	this->omni->color = RandomGenerator::random() * 0xFFFFFF;
#else
	(void)container;
#endif
}

ClientObjectUser *PlasmaShot::getOwner()
{
	return nullptr;
}

bool PlasmaShot::play(int millis, GameCamera *camera)
{
	(void)camera;

	if (this->totalDistance > this->plasmaData->shotRange)
	{
		this->listener->plasmaShotDissolved(this);
		return false;
	}

	double dt = 0.001 * millis;
	double distance = this->plasmaData->shotSpeed * dt;
	this->totalDistance = this->totalDistance + distance;
	this->sprite->update(dt);

	if (this->active)
	{
		RayIntersection _rayHit;
		if (this->collisionDetector->intersectRay(&this->currPosition, &this->direction, CollisionGroup::WEAPON, distance, this, &_rayHit))
		{
			//const Vector3*, const Vector3*, int, double, IRayCollisionPredicate*, RayIntersection*
			if (_rayHit.primitive != nullptr)
			{
				this->listener->plasmaShotHit(this, &_rayHit.pos, &this->direction, _rayHit.primitive->body);
			}
			return false;
		}

		int i = 0;
		while (i < NUM_RADIAL_RAYS)
		{
			Vector3 *p = this->radialPoints[i];
			if (this->collisionDetector->intersectRay(p, &this->direction, CollisionGroup::WEAPON, distance, this, &_rayHit))
			{
				if (_rayHit.primitive->body != nullptr)
				{
					this->listener->plasmaShotHit(this, &_rayHit.pos, &this->direction, _rayHit.primitive->body);
					return false;
				}
			}
			i++;
		}

		Vector3 _rayOrigin;
		_rayOrigin.x = this->currPosition.x;
		_rayOrigin.y = this->currPosition.y;
		_rayOrigin.z = this->currPosition.z + this->plasmaData->shotRadius;

		Vector3 _rayDirection;
		_rayDirection.x = 0;
		_rayDirection.y = 0;
		_rayDirection.z = -1;
		if (this->collisionDetector->intersectRayWithStatic(&_rayOrigin, &_rayDirection, CollisionGroup::STATIC, this->plasmaData->shotRadius, nullptr, &_rayHit))
		{
			this->listener->plasmaShotHit(this, &this->currPosition, &this->direction, nullptr);
			return false;
		}
	}

	double dx = (distance * this->direction.x);
	double dy = (distance * this->direction.y);
	double dz = (distance * this->direction.z);
	this->currPosition.x = (this->currPosition.x + dx);
	this->currPosition.y = (this->currPosition.y + dy);
	this->currPosition.z = (this->currPosition.z + dz);
	int i = 0;
	while (i < NUM_RADIAL_RAYS)
	{
		Vector3 *p = this->radialPoints[i];
		p->x = p->x + dx;
		p->y = p->y + dy;
		p->z = p->z + dz;
		i++;
	}

	double size = SIZE * WeaponWeakeningModel::getImpactCoeff(this->shooterData->turret, 0.01 * this->totalDistance);
	this->sprite->width = size;
	this->sprite->height = size;
	this->sprite->position.x = this->currPosition.x;
	this->sprite->position.y = this->currPosition.y;
	this->sprite->position.z = this->currPosition.z;
	/*
	if (this->battleField->gameMode == GameModes.HALLOWEEN)
	{
		this->omni.x = this->currPosition.x;
	}
	*/
#ifdef GRAPHICS
	this->omni->position.y = this->currPosition.y;
	this->omni->position.z = this->currPosition.z;
#endif
	this->sprite->rotation -= 0.003 * millis;

	return true;
}


void PlasmaShot::destroy()
{
#ifdef GRAPHICS
	this->sprite->getParent()->removeChild(this->sprite.get());
#endif

	/*
	if (this->battleField.gameMode == GameModes.HALLOWEEN)
	{
		this->omni.parent.removeChild(this->omni);
	}
	*/
	this->plasmaData = nullptr;
	this->listener = nullptr;
	this->collisionDetector = nullptr;
	this->shooterData = nullptr;
	this->weakeningModel = nullptr;
	this->sfxData = nullptr;
	this->sprite->material = nullptr;
	this->sprite->colorTransform = nullptr;
	//var _local_1:* = ++poolIndex;
	//pool[_local_1] = this;
	this->storeInPool();
}

void PlasmaShot::kill()
{
	this->totalDistance = this->plasmaData->shotRange + 1;
}

bool PlasmaShot::considerBody(Body *body)
{
	return (!(this->shooterData->tank == body));
}

void PlasmaShot::initRadialPoints(const Vector3 *centerPoint, const Vector3 *shotDirection, double shotRadius)
{
	int axis = 0;
	double min = 1E308;
	double d = ((shotDirection->x < 0) ? -(shotDirection->x) : shotDirection->x);
	if (d < min)
	{
		min = d;
		axis = 0;
	}
	d = ((shotDirection->y < 0) ? -(shotDirection->y) : shotDirection->y);
	if (d < min)
	{
		min = d;
		axis = 1;
	}
	d = ((shotDirection->z < 0) ? -(shotDirection->z) : shotDirection->z);
	if (d < min)
	{
		axis = 2;
	}

	Vector3 v;
	switch (axis)
	{
		case 0:
			v.x = 0;
			v.y = shotDirection->z;
			v.z = -(shotDirection->y);
			break;
		case 1:
			v.x = -(shotDirection->z);
			v.y = 0;
			v.z = shotDirection->x;
			break;
		case 2:
			v.x = shotDirection->y;
			v.y = -(shotDirection->x);
			v.z = 0;
			break;
	}

	v.normalize();
	v.scale(shotRadius);
	Matrix3 _rotationMatrix;
	_rotationMatrix.fromAxisAngle(shotDirection, RADIAL_ANGLE_STEP);


	this->radialPoints[0]->copy(centerPoint);
	this->radialPoints[0]->add(&v);


	for (int i = 0; i < NUM_RADIAL_RAYS; i++)
	{
		v.transformBy3(&_rotationMatrix);
		this->radialPoints[i]->copy(centerPoint);
		this->radialPoints[i]->add(&v);
	}
}
