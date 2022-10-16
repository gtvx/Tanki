#ifndef HEALINGGUNSFX_H
#define HEALINGGUNSFX_H

#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "IHealingGunEffectListener.h"

class TankData;
class HealingGunGraphicEffect;
class IBattleField;
enum class IsisActionType;
class Vector3;
class HealingGunSFXData;
class Object3D;
class HealingGunCameraEffect;


class HealingGunSFX : public PooledObject, public IHealingGunEffectListener
{	
	HealingGunCameraEffect *cameraEffect;

	HealingGunGraphicEffect *graphicEffect;
	//HealingGunSoundEffect *soundEffect;
	IBattleField *battlefield;
	bool graphicEffectDestroyed;
	bool soundEffectDestroyed;
public:

	TankData *targetData;

	HealingGunSFX(ObjectPool *objectPool);

	void init(IsisActionType mode,
			  HealingGunSFXData *sfxData,
			  Object3D *turret,
			  const Vector3 *localSourcePosition,
			  TankData *tankData);

	void setMode(IsisActionType value);

	void addToBattlefield(IBattleField *battlefield);

	void destroy();

	void setTargetParams(TankData *targetData,
						 TankData *ownerData,
						 Object3D *target,
						 const Vector3 *localTargetPosition,
						 IsisActionType mode);

	void onEffectDestroyed(ISpecialEffect *effect);

private:
	bool isLocalTarget();
};

#endif // HEALINGGUNSFX_H
