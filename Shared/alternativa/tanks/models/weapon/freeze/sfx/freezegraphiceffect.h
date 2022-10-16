#ifndef FREEZEGRAPHICEFFECT_H
#define FREEZEGRAPHICEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>
#include <QVector>


class TankData;
class Vector3;
class Object3D;
class FreezeSFXData;
class ICollisionDetector;
class AnimatedPlane;
class Particle;
class RayIntersection;


class FreezeGraphicEffect : public IGraphicEffect, public PooledObject
{
	Vector3 *localMuzzlePosition;
	Object3D *turret;
	FreezeSFXData *sfxData;
	ICollisionDetector *collisionDetector;
	QVector<std::shared_ptr<Particle>> particles;
	Scene3DContainer *container;
	std::shared_ptr<AnimatedPlane> plane;
	Vector3 *barrelOrigin;
	Vector3 *direction;
	Vector3 *globalMuzzlePosition;
	Vector3 *turretAxisX;
	RayIntersection *intersection;
	TankData *shooterData;
	double range;
	double coneHalfAngleTan;
	double particleSpeed;
	double particleScalePerDistance;
	double particleEmissionPeriod;
	int numParticleFrames;
	int time;
	int nextEmissionTime;
	int numParticles;
	bool dead;

public:
	FreezeGraphicEffect(ObjectPool *objectPool);

	void init(TankData *shooterData,
			  double range,
			  double coneAngle,
			  double particleSpeed,
			  const Vector3 *localMuzzlePosition,
			  Object3D *turret,
			  FreezeSFXData *sfxData,
			  ICollisionDetector *collisionDetector);

	void destroy() override;
	ClientObjectUser* getOwner() override;
	bool play(int millis, GameCamera *camera) override;
	void kill() override;
	void addToContainer(Scene3DContainer *container) override;

private:
	void calculateParameters();
	void addParticle();
	void removeParticle(int index);
};

#endif // FREEZEGRAPHICEFFECT_H
