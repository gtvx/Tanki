#ifndef PLASMASHOT_H
#define PLASMASHOT_H

#include <QVector>
#include <memory>
#include "alternativa/math/vector3.h"
#include "alternativa/physics/collision/IRayCollisionPredicate.h"
#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"


class OmniLight;
class IPlasmaShotListener;
class PlasmaGunData;
class ICollisionDetector;
class AnimatedSprite3D;
class BattlefieldModel;
class IWeaponWeakeningModel;
class PlasmaSFXData;
class TankData;
class Scene3DContainer;
class ClientObject;
class GameCamera;
class Body;



class PlasmaShot : public IGraphicEffect, public IRayCollisionPredicate, public PooledObject
{
	QVector<Vector3*> radialPoints;
	Vector3 currPosition;
	Vector3 direction;
	IPlasmaShotListener *listener;
	PlasmaGunData *plasmaData;
	ICollisionDetector *collisionDetector;
	std::shared_ptr<AnimatedSprite3D> sprite;
	int numFrames;
	double currentFrame;
	BattlefieldModel *battleField;
	IWeaponWeakeningModel *weakeningModel;
	PlasmaSFXData *sfxData;
	std::shared_ptr<OmniLight> omni;

public:

	static const double SIZE;

	int shotId;
	bool active;
	TankData *shooterData;
	double totalDistance;

	PlasmaShot(ObjectPool*);


	void init(int shotId,
			  bool active,
			  PlasmaGunData *plasmaData,
			  const Vector3 *startPosition,
			  const Vector3 *direction,
			  TankData *shooterData,
			  IPlasmaShotListener *listener,
			  PlasmaSFXData *sfxData,
			  ICollisionDetector *collisionDetector,
			  IWeaponWeakeningModel *weakeningModel);

	void addToContainer(Scene3DContainer *container) override;

	ClientObjectUser* getOwner() override;

	bool play(int millis, GameCamera *camera);
	void destroy() override;
	void kill() override;
	bool considerBody(Body *body) override;

	//public function toString():String
		//return (((("[PlasmaCharge tankId=" + this.shooterData.user.id) + ", chargeId=") + this.shotId) + "]");

private:
	void initRadialPoints(const Vector3 *centerPoint, const Vector3 *shotDirection, double shotRadius);
};

#endif // PLASMASHOT_H
