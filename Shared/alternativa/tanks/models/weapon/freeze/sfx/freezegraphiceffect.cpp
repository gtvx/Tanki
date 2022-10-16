#include "freezegraphiceffect.h"
#include "animatedplane.h"
#include "mymath.h"
#include "flash/geom/colortransform.h"
#include "alternativa/tanks/models/sfx/colortransform/colortransformentry.h"
#include "freezesfxdata.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "../../../sfx/particle.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/tanks/sfx/sfxutils.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "randomgenerator.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"



static const double PARTICLE_ANIMATION_FPS = 30;
static const double PARTICLE_START_SCALE = 0.5;
static const double PARTICLE_END_SCALE = 3;
static const int PLANE_LENGTH = 350;
static const int PLANE_WIDTH = 210;
static const double PARTICLE_SIZE = 150;
static const int MAX_PARTICLES = 20;
static const double PARTICLE_ROTATON_SPEED = 3;
//static var matrix:Matrix3 = new Matrix3();
//static var turretMatrix:Matrix4 = new Matrix4();
//static var barrelOrigin:Vector3 = new Vector3();
//static var direction:Vector3 = new Vector3();
//static var turretAxisX:Vector3 = new Vector3();
//static var particlePosition:Vector3 = new Vector3();
//static var globalMuzzlePosition:Vector3 = new Vector3();
//static var intersection:RayIntersection = new RayIntersection();
static const int collisionGroup = 16;


FreezeGraphicEffect::FreezeGraphicEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->plane = new_AnimatedPlane(PLANE_WIDTH, PLANE_LENGTH);
	this->localMuzzlePosition = new Vector3();
	this->barrelOrigin = new Vector3();
	this->direction = new Vector3();
	this->globalMuzzlePosition = new Vector3();
	this->turretAxisX = new Vector3();
	this->intersection = new RayIntersection();
	this->intersection->t = 0;
	this->numParticles = 0;
}

void FreezeGraphicEffect::init(TankData *shooterData, double range, double coneAngle, double particleSpeed, const Vector3 *localMuzzlePosition, Object3D *turret, FreezeSFXData *sfxData, ICollisionDetector *collisionDetector)
{
	this->shooterData = shooterData;
	this->range = range;
	this->coneHalfAngleTan = MyMath::tan((0.5 * coneAngle));
	this->particleSpeed = particleSpeed;
	this->localMuzzlePosition->copy(localMuzzlePosition);
	this->turret = turret;
	this->sfxData = sfxData;
	this->collisionDetector = collisionDetector;
	this->particleScalePerDistance = (2 * (PARTICLE_END_SCALE - PARTICLE_START_SCALE)) / range;
	this->numParticleFrames = sfxData->particleMaterials->length();
	this->particleEmissionPeriod = ((1000 * range) / (MAX_PARTICLES * particleSpeed));
	this->numParticles = 0;
	this->time = this->nextEmissionTime = 0; //getTimer();
	this->plane->setMaterials(sfxData->planeMaterials);
	this->plane->currFrame = 0;
	if (!sfxData->colorTransformPoints.isEmpty())
	{
		ColorTransformEntry *ctp = sfxData->colorTransformPoints.at(0);
		std::shared_ptr<ColorTransform> colorTransform = ((this->plane->colorTransform == nullptr) ? std::make_shared<ColorTransform>() : this->plane->colorTransform);
		colorTransform->alphaMultiplier = ctp->alphaMultiplier;
		colorTransform->alphaOffset = ctp->alphaOffset;
		colorTransform->redMultiplier = ctp->redMultiplier;
		colorTransform->redOffset = ctp->redOffset;
		colorTransform->greenMultiplier = ctp->greenMultiplier;
		colorTransform->greenOffset = ctp->greenOffset;
		colorTransform->blueMultiplier = ctp->blueMultiplier;
		colorTransform->blueOffset = ctp->blueOffset;
		this->plane->colorTransform = colorTransform;
	}
	else
	{
		this->plane->colorTransform = nullptr;
	}
	this->dead = false;
}

void FreezeGraphicEffect::destroy()
{
	while (this->numParticles > 0)
	{
		this->removeParticle(0);
	}

	//this->plane->removeFromParent();
	//this->plane->clearMaterials();
	//this->plane->destroy();
	//this->plane = nullptr;
	this->container = nullptr;
	this->shooterData = nullptr;
	this->turret = nullptr;
	this->sfxData = nullptr;
	this->collisionDetector = nullptr;
	this->storeInPool();
}

ClientObjectUser *FreezeGraphicEffect::getOwner()
{
	return nullptr;
}


bool FreezeGraphicEffect::play(int millis, GameCamera *camera)
{
	this->calculateParameters();
	double dt = 0.001 * millis;

	if (this->collisionDetector->intersectRayWithStatic(barrelOrigin,
														direction,
														CollisionGroup::STATIC,
														this->localMuzzlePosition->y + PLANE_WIDTH,
														nullptr,
														intersection))
	{
		this->plane->visible = false;
	}
	else
	{
		this->plane->visible = true;
		this->plane->update(dt, 25);
		SFXUtils::alignObjectPlaneToView(this->plane.get(), globalMuzzlePosition, direction, &camera->pos);
	}

	if ((((!(this->dead)) && (this->numParticles < MAX_PARTICLES)) && (this->time >= this->nextEmissionTime)))
	{
		this->nextEmissionTime = (this->nextEmissionTime + this->particleEmissionPeriod);
		this->addParticle();
	}

	int i = 0;
	while (i < this->numParticles)
	{
		Particle *particle = this->particles.at(i).get();
		//particlePosition.x = particle->x;
		//particlePosition.y = particle->y;
		//particlePosition.z = particle->z;
		if (particle->distance > this->range ||
			this->collisionDetector->intersectRayWithStatic(&particle->position,
															&particle->velocity,
															collisionGroup,
															dt,
															nullptr,
															intersection))
		{
			this->removeParticle(i--);
		}
		else
		{
			const Vector3 *velocity = &particle->velocity;
			particle->position.x = (particle->position.x + (velocity->x * dt));
			particle->position.y = (particle->position.y + (velocity->y * dt));
			particle->position.z = (particle->position.z + (velocity->z * dt));
			particle->distance = (particle->distance + (this->particleSpeed * dt));
			particle->rotation = (particle->rotation + (PARTICLE_ROTATON_SPEED * dt));
			particle->material = this->sfxData->particleMaterials->at(int(particle->currFrame));
			particle->currFrame = (particle->currFrame + (PARTICLE_ANIMATION_FPS * dt));
			if (particle->currFrame >= this->numParticleFrames)
			{
				particle->currFrame = 0;
			};
			double scale = (PARTICLE_START_SCALE + (this->particleScalePerDistance * particle->distance));
			if (scale > PARTICLE_END_SCALE)
			{
				scale = PARTICLE_END_SCALE;
			};
			double size = PARTICLE_SIZE * scale;
			particle->width = size;
			particle->height = size;
			particle->updateColorTransofrm(this->range, this->sfxData->colorTransformPoints);
		}
		i++;
	}

	this->time = this->time + millis;
	return this->dead == false || (this->numParticles > 0);
}

void FreezeGraphicEffect::kill()
{
	if (this->dead == false)
	{
		this->dead = true;
		this->plane->removeFromParent();
	}
}

void FreezeGraphicEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->plane.get());
}

void FreezeGraphicEffect::calculateParameters()
{
	Matrix4 turretMatrix;
	turretMatrix.setMatrix(&this->turret->position, &this->turret->rotation);
	turretAxisX->x = turretMatrix.a;
	turretAxisX->y = turretMatrix.e;
	turretAxisX->z = turretMatrix.i;
	direction->x = turretMatrix.b;
	direction->y = turretMatrix.f;
	direction->z = turretMatrix.j;
	turretMatrix.transformVector(this->localMuzzlePosition, globalMuzzlePosition);
	double barrleLength = this->localMuzzlePosition->y;
	barrelOrigin->x = globalMuzzlePosition->x - (barrleLength * direction->x);
	barrelOrigin->y = globalMuzzlePosition->y - (barrleLength * direction->y);
	barrelOrigin->z = globalMuzzlePosition->z - (barrleLength * direction->z);
}



void FreezeGraphicEffect::addParticle()
{
	double offset = RandomGenerator::random() * 50;
	if (((!(this->plane->visible)) && (intersection->t < (this->localMuzzlePosition->y + offset))))
	{
		return;
	}

	std::shared_ptr<Particle> particle = new_Particle(nullptr);
	particle->rotation = ((RandomGenerator::random() * MyMath::PI) * 2);
	particle->currFrame = (RandomGenerator::random() * this->numParticleFrames);
	double angle = (2 * MyMath::PI) * RandomGenerator::random();

	Matrix3 matrix;
	matrix.fromAxisAngle(direction, angle);
	turretAxisX->transformBy3(&matrix);
	double d = ((this->range * this->coneHalfAngleTan) * RandomGenerator::random());
	direction->x = ((direction->x * this->range) + (turretAxisX->x * d));
	direction->y = ((direction->y * this->range) + (turretAxisX->y * d));
	direction->z = ((direction->z * this->range) + (turretAxisX->z * d));
	direction->normalize();
	particle->velocity.x = (this->particleSpeed * direction->x);
	particle->velocity.y = (this->particleSpeed * direction->y);
	particle->velocity.z = (this->particleSpeed * direction->z);
	particle->velocity.add(&this->shooterData->tank->state.velocity);
	particle->distance = offset;
	particle->position.x = (globalMuzzlePosition->x + (offset * direction->x));
	particle->position.y = (globalMuzzlePosition->y + (offset * direction->y));
	particle->position.z = (globalMuzzlePosition->z + (offset * direction->z));
	this->particles.append(particle);
	this->numParticles++;
	this->container->addChild(particle.get());
}

void FreezeGraphicEffect::removeParticle(int index)
{
	std::shared_ptr<Particle> particle = this->particles.at(index);
	this->particles[index] = this->particles[--this->numParticles];
	//this->particles[this->numParticles] = nullptr;
	this->particles.removeLast();
	particle->dispose();
	particle->destroy();
	particle = nullptr;
}
