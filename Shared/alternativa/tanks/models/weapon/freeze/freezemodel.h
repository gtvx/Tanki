#ifndef FREEZEMODEL_H
#define FREEZEMODEL_H

#include "../IWeaponController.h"
#include <qglobal.h>

class QString;
class ClientObject;
class TankModel;
class IBattleField;
class IWeaponCommonModel;
class IFreezeSFXModel;
class WeaponCommonData;
class ConicAreaTargetSystem;
class FreezeData;
class ClientObjectWeapon;
class FreezeItemHitUser;


class FreezeModel : public IWeaponController
{
	IBattleField *battlefieldModel;
	TankModel *tankModel;
	IWeaponCommonModel *weaponCommonModel;
	IFreezeSFXModel *sfxModel;
	TankData *localTankData;
	FreezeData *localFreezeData;
	WeaponCommonData *localWeaponCommonData;
	double currentEnergy;
	int nextTargetCheckTime;
	int lastUpdateTime;
	bool active;
	ConicAreaTargetSystem *targetSystem;
	QVector<FreezeItemHitUser*> *targets;

public:
	FreezeModel();
	virtual ~FreezeModel();


	void startFire(ClientObject *clientObject, const QString &shooterId);

	void stopFire(ClientObject *clientObject, const QString &shooterId);

	void initObject(ClientObjectWeapon *clientObject,
					double damageAreaConeAngle,
					double damageAreaRange,
					int energyCapacity,
					int energyDischargeSpeed,
					int energyRechargeSpeed,
					int weaponTickMsec);

	void stopEffects(TankData *ownerTankData) override;
	void reset() override;
	void setLocalUser(TankData *localUserData) override;
	void clearLocalUser() override;
	void activateWeapon(int time) override;
	void deactivateWeapon(int time, bool sendServerCommand) override;
	double update(int time, int deltaTime);

	FreezeData *getFreezeData(ClientObjectWeapon *clientObject);

private:
	void checkTargets(WeaponCommonData *commonData, TankData *tankData);
	void createEffects(TankData *tankData, WeaponCommonData *commonData);
	TankData* getTankDataSafe(const QString &tankId);
	void cacheInterfaces();
	void startFireCommand();
	void stopFireCommand();
	void hitCommand(QVector<FreezeItemHitUser*> *targets, int len);
};

#endif // FREEZEMODEL_H
