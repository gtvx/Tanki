#ifndef RICOCHETMODEL_H
#define RICOCHETMODEL_H

#include "../IWeaponController.h"
#include "IRicochetShotListener.h"

class QString;
class Vector3;
class ClientObjectUser;
class ClientObjectWeapon;
class RicochetShot;
class Body;
class RicochetSFXData;
class Sound;
class WeaponCommonData;
class RicochetShot;
class RicochetData;
class ShotData;
class RicochetTargetSystem;
class TankModel;
class IBattleField;
class IWeaponCommonModel;
class IRicochetSFXModel;
class ObjectPoolService;


class RicochetModel : public IWeaponController, public IRicochetShotListener
{
	IWeaponCommonModel *weaponCommonModel;
	IBattleField *battlefield;
	TankModel *tankModel;
	IRicochetSFXModel *ricochetSfxModel;
	TankData *localTankData;
	RicochetData *localRicochetData;
	RicochetSFXData *localRicochetSFXData;
	ShotData *localShotData;
	WeaponCommonData *localWeaponCommonData;
	bool triggerPressed;
	int readyTime;
	double currentEnergy;
	RicochetTargetSystem *targetSystem;
	ObjectPoolService *objectPoolService;

public:
	RicochetModel();
	virtual ~RicochetModel();

	void hit(ClientObjectUser *clientObject,
			 const QString &targetId,
			 const Vector3 *hitPoint,
			 int hitDirectionX,
			 int hitDirectionY,
			 int hitDirectionZ,
			 double weakeningCoeff);

	void fire(ClientObjectUser *clientObject, const QString &shooterId, int shotDirectionX, int shotDirectionY, int shotDirectionZ);
	void hitSelf(ClientObjectWeapon *clientObject, const QString &shooterId, int relativeHitPosition);

	void initObject(ClientObjectWeapon *clientObject,
					int energyCapacity,
					int energyPerShot,
					double energyRechargeSpeed,
					double shotDistance,
					double shotRadius,
					double shotSpeed);

	void objectLoaded(ClientObjectWeapon *clientObject);
	void objectUnloaded(ClientObjectWeapon *object);
	void stopEffects(TankData *ownerTankData);
	void reset();
	void setLocalUser(TankData *localUserData);
	void clearLocalUser();
	void activateWeapon(int time);
	void deactivateWeapon(int time, bool sendServerCommand);
	double update(int time, int deltaTime);
	void shotHit(RicochetShot *shot, const Vector3 *hitPoint, const Vector3 *hitDir, Body *targetBody) override;

private:
	void  hitCommand(const QString &victimId,
					 int shotId,
					 int inc,
					 const Vector3 *hitPos3d,
					 const Vector3 *targetPos3d,
					 double x,
					 double y,
					 double z,
					 double distance);

	void createExplosion(const Vector3 *position, RicochetSFXData *sfxData);

	void addSoundEffect(Sound *sound,
						const Vector3 *position,
						double nearRadius,
						double farRadius,
						double farDelimiter,
						double soundVolume);

	void doFire(int time, WeaponCommonData *commonData);
	void hitSelfCommand(int inc, int relativeHitPos, const Vector3 *pos3d);
	void fireCommand(int shotid, double x, double y, double z);

	RicochetShot* createShotEffects(TankData *shooterData,
									RicochetData *ricochetData,
									RicochetSFXData *ricochetSFXData,
									WeaponCommonData *weaponCommonData,
									const Vector3 *localMuzzlePosition,
									const Vector3 *globalMuzzlePosition,
									const Vector3 *shotDirection,
									const Vector3 *gunDirection);

	void createSelfHitEffects(TankData *shooterData,
							  RicochetSFXData *ricochetSFXData,
							  WeaponCommonData *weaponCommonData,
							  int relativeHitPos,
							  const Vector3 *globalBarrelOrigin,
							  const Vector3 *gunDirection);

	TankData* getTankDataSafe(const QString &objectId);
};

#endif // RICOCHETMODEL_H
