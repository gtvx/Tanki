#ifndef FLAMETHROWERGRAPHICEFFECT_H
#define FLAMETHROWERGRAPHICEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <QVector>
#include <memory>

class Object3D;
class FlamethrowerSFXData;
class TanksCollisionDetector;
class IWeaponWeakeningModel;
class TankData;
class TextureAnimation;
class Particle;

class FlamethrowerGraphicEffect : public IGraphicEffect, public PooledObject
{
	double range;
	double scalePerDistance;
	double coneHalfAngleTan;
	double particleSpeed;

	int maxParticles;
	int numParticles;
	int numFrames;
	int emissionDelta;
	int nextEmissionTime;
	int time;
	int collisionGroup;

	bool dead;

	Vector3 localMuzzlePosition;
	QVector<std::shared_ptr<Particle>> particles;

	FlamethrowerSFXData *sfxData;
	Scene3DContainer *container;
	TanksCollisionDetector *collisionDetector;
	IWeaponWeakeningModel *weakeningModel;
	TankData *shooterData;
	std::shared_ptr<TextureAnimation> animatedTexture;
	Object3D *turret;

public:
	FlamethrowerGraphicEffect(ObjectPool*);
	void init(TankData *shooterData, double range, double coneAngle, int maxParticles, double particleSpeed, const Vector3 *muzzleLocalPos, Object3D *turret, FlamethrowerSFXData *sfxData, TanksCollisionDetector *collisionDetector, IWeaponWeakeningModel *weakeningModel);
	ClientObjectUser *getOwner() override;
	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;

private:
	void tryToAddParticle();
	void addParticle(Vector3 *globalMuzzlePosition, Vector3 *direction, Vector3 *gunAxisX, double offset);
	void removeParticle(int index);
};

#endif // FLAMETHROWERGRAPHICEFFECT_H
