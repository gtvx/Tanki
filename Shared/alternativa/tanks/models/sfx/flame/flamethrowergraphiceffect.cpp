#include "flamethrowergraphiceffect.h"
#include "../../sfx/particle.h"
#include "flamethrowersfxdata.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "mymath.h"
#include "flash/utils/gettimer.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "randomgenerator.h"


static const double START_SCALE = 0.5;
static const double END_SCALE = 4;
static double particleBaseSize = 100;



FlamethrowerGraphicEffect::FlamethrowerGraphicEffect(ObjectPool *objectPool)
	: PooledObject(objectPool, 8)
{
	collisionGroup = 16;
}


void FlamethrowerGraphicEffect::init(TankData *shooterData,
									 double range,
									 double coneAngle,
									 int maxParticles,
									 double particleSpeed,
									 const Vector3 *muzzleLocalPos,
									 Object3D *turret,
									 FlamethrowerSFXData *sfxData,
									 TanksCollisionDetector *collisionDetector,
									 IWeaponWeakeningModel *weakeningModel)
{
	this->numParticles = 0;
	this->shooterData = shooterData;
	this->range = range;
	this->scalePerDistance = ((2 * (END_SCALE - START_SCALE)) / range);
	this->coneHalfAngleTan = MyMath::tan(0.5 * coneAngle);
	this->maxParticles = maxParticles;
	this->particleSpeed = particleSpeed;
	this->localMuzzlePosition.copy(muzzleLocalPos);
	this->turret = turret;
	this->sfxData = sfxData;
	this->collisionDetector = collisionDetector;
	this->weakeningModel = weakeningModel;
	this->numFrames = sfxData->materials->length();
	this->emissionDelta = (1000 * range) / (maxParticles * particleSpeed);
	this->time = this->nextEmissionTime = getTimer();
	this->particles.reserve(maxParticles);
	this->dead = false;
	this->animatedTexture = sfxData->data;
}

ClientObjectUser* FlamethrowerGraphicEffect::getOwner()
{
	return nullptr;
}

void FlamethrowerGraphicEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
}

bool FlamethrowerGraphicEffect::play(int millis, GameCamera *camera)
{
	(void)camera;

	if ((!this->dead && (this->numParticles < this->maxParticles)) && (this->time >= this->nextEmissionTime))
	{
		this->nextEmissionTime = (this->nextEmissionTime + this->emissionDelta);
		this->tryToAddParticle();
	}

	double dt = 0.001 * millis;

	for (int i = 0; i < this->numParticles; i++)
	{
		Particle *particle = this->particles[i].get();
		Vector3 particlePosition;
		particlePosition.x = particle->position.x;
		particlePosition.y = particle->position.y;
		particlePosition.z = particle->position.z;

		RayIntersection intersection;
		if (((particle->distance > this->range) || (this->collisionDetector->intersectRayWithStatic(&particlePosition, &particle->velocity, this->collisionGroup, dt, nullptr, &intersection))))
		{
			this->removeParticle(i--);
		}
		else
		{
			Vector3 *velocity = &particle->velocity;
			particle->position.x = particle->position.x + (dt * velocity->x);
			particle->position.y = particle->position.y + (dt * velocity->y);
			particle->position.z = particle->position.z + (dt * velocity->z);
			particle->distance = particle->distance + (this->particleSpeed * dt);
			particle->update(dt);
			double scale = (START_SCALE + (this->scalePerDistance * particle->distance));
			if (scale > END_SCALE)
				scale = END_SCALE;

			double size = scale * particleBaseSize;
			particle->width = size;
			particle->height = size;
			particle->updateColorTransofrm(this->range, this->sfxData->colorTransformPoints);
		}
	}

	this->time = (this->time + millis);
	return !this->dead || (this->numParticles > 0);
}

void FlamethrowerGraphicEffect::destroy()
{
	while (this->numParticles > 0)
		this->removeParticle(0);

	this->collisionDetector = nullptr;
	this->turret = nullptr;
	this->shooterData = nullptr;
	this->sfxData = nullptr;
	this->storeInPool();
}

void FlamethrowerGraphicEffect::kill()
{
	this->dead = true;
}


void FlamethrowerGraphicEffect::tryToAddParticle()
{
	Matrix3 matrix;
	Vector3 barrelOrigin;

	matrix.setRotationMatrix(this->turret->rotation.x, this->turret->rotation.y, this->turret->rotation.z);
	barrelOrigin.x = 0;
	barrelOrigin.y = 0;
	barrelOrigin.z = this->localMuzzlePosition.z;
	barrelOrigin.transformBy3(&matrix);
	barrelOrigin.x = barrelOrigin.x + this->turret->position.x;
	barrelOrigin.y = barrelOrigin.y + this->turret->position.y;
	Vector3 gunDirection;
	barrelOrigin.z = barrelOrigin.z + this->turret->position.z;
	gunDirection.x = matrix.b;
	gunDirection.y = matrix.f;
	gunDirection.z = matrix.j;
	double offset = RandomGenerator::random() * 50.;
	RayIntersection intersection;
	if (!this->collisionDetector->intersectRayWithStatic(&barrelOrigin, &gunDirection, CollisionGroup::STATIC, this->localMuzzlePosition.y + offset, nullptr, &intersection))
	{
		double barrelLength = this->localMuzzlePosition.y;
		Vector3 globalMuzzlePosition;
		globalMuzzlePosition.x = (barrelOrigin.x + (gunDirection.x * barrelLength));
		globalMuzzlePosition.y = (barrelOrigin.y + (gunDirection.y * barrelLength));
		globalMuzzlePosition.z = (barrelOrigin.z + (gunDirection.z * barrelLength));
		Vector3 xAxis;
		xAxis.x = matrix.a;
		xAxis.y = matrix.e;
		xAxis.z = matrix.i;
		this->addParticle(&globalMuzzlePosition, &gunDirection, &xAxis, offset);
	}
}




void FlamethrowerGraphicEffect::addParticle(Vector3 *globalMuzzlePosition, Vector3 *direction, Vector3 *gunAxisX, double offset)
{
	Matrix3 matrix;
	std::shared_ptr<Particle> particle = new_Particle(this->animatedTexture.get());
	particle->currFrame = RandomGenerator::random() * this->numFrames;
	double angle = (2 * MyMath::PI) * RandomGenerator::random();
	matrix.fromAxisAngle(direction, angle);
	gunAxisX->transformBy3(&matrix);
	double d = ((this->range * this->coneHalfAngleTan) * RandomGenerator::random());
	direction->x = ((direction->x * this->range) + (gunAxisX->x * d));
	direction->y = ((direction->y * this->range) + (gunAxisX->y * d));
	direction->z = ((direction->z * this->range) + (gunAxisX->z * d));
	direction->normalize();
	particle->velocity.x = (this->particleSpeed * direction->x);
	particle->velocity.y = (this->particleSpeed * direction->y);
	particle->velocity.z = (this->particleSpeed * direction->z);
	particle->velocity.add(&this->shooterData->tank->state.velocity);
	particle->distance = offset;
	particle->position.x = (globalMuzzlePosition->x + (offset * direction->x));
	particle->position.y = (globalMuzzlePosition->y + (offset * direction->y));
	particle->position.z = (globalMuzzlePosition->z + (offset * direction->z));
	//int _local_8  = this->numParticles++;
	//this->particles[_local_8] = particle;
	this->particles.append(particle);
	this->numParticles = this->particles.length();
	this->container->addChild(particle.get());
}

void FlamethrowerGraphicEffect::removeParticle(int index)
{
	std::shared_ptr<Particle> particle = this->particles[index];

	if (this->particles.length() > 0)
	{
		if (index == this->particles.length()-1)
		{
			this->particles.removeLast();
		}
		else
		{
			this->particles[index] = this->particles.takeLast();
		}
	}

	particle->dispose();
	particle->destroy();
	particle = nullptr;
	this->numParticles = this->particles.length();
}
