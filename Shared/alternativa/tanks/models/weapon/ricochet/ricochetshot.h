#ifndef RICOCHETSHOT_H
#define RICOCHETSHOT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/physics/collision/IRayCollisionPredicate.h"
#include "alternativa/math/vector3.h"
#include <memory>
#include <QVector>

class Vector3;
class TankData;
class RicochetData;
class RicochetSFXData;
class ICollisionDetector;
class IWeaponWeakeningModel;
class IRicochetShotListener;
class IBattleField;
class Sound;
class AnimatedSprite3D;
class TailTrail;
class BattlefieldModel;
class OmniLight;


class RicochetShot : public IGraphicEffect, public PooledObject, public IRayCollisionPredicate
{
	Vector3 radialPoints[6];
	Vector3 currPos;
	Vector3 direction;
	IRicochetShotListener *listener;
	RicochetData *ricochetData;
	ICollisionDetector *collisionDetector;
	std::shared_ptr<AnimatedSprite3D> sprite;
	int numFrames;
	double currFrame;
	BattlefieldModel *battleField;
	std::shared_ptr<OmniLight> omni;
	int ricochetCount;
	std::shared_ptr<TailTrail> tailTrail;
	bool isFirstFrame;
public:

	int id;
	TankData *shooterData;
	double totalDistance = 0;
	RicochetSFXData *sfxData;

	RicochetShot(ObjectPool *objectPool);

	void init(const Vector3 *startPos,
			  const Vector3 *direction,
			  TankData *shooterData,
			  RicochetData *ricochetData,
			  RicochetSFXData *sfxData,
			  ICollisionDetector *collisionDetector,
			  IRicochetShotListener *listener,
			  IBattleField *battlefield);

	void addToContainer(Scene3DContainer *container) override;
	ClientObjectUser* getOwner() override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;
	bool considerBody(Body *body) override;

private:
	void initRadialPoints(const Vector3 *centerPoint, const Vector3 *dir, double radius);

	void addSoundEffect(Sound *sound,
						const Vector3 *position,
						double nearRadius,
						double farRadius,
						double farDelimiter,
						double soundVolume);
};

#endif // RICOCHETSHOT_H
