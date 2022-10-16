#ifndef HEALINGGUNMODEL_H
#define HEALINGGUNMODEL_H

#include "../IWeaponController.h"
#include "alternativa/tanks/models/tank/ITankEventListener.h"
#include <QVector>

class ClientObjectWeapon;
class IsisAction;
class QString;
class Array;
class Vector3;
class WeaponCommonData;
class HealingGunData;
class IWeaponCommonModel;
class HealingGunTargetSystem;
class TankModel;
class HealingGunSFXModel;
class IBattleField;
class ClientObjectUser;


class HealingGunModel : public IWeaponController, public ITankEventListener
{
	//modelService:IModelService;
	IBattleField *bfInterface;
	TankModel *tankInterface;
	IWeaponCommonModel *weaponCommonInterface;
	HealingGunSFXModel *sfxModel;
	TankData *currTarget;
	HealingGunTargetSystem *targetSystem;
	bool active;
	TankData *localTankData;
	WeaponCommonData *localCommonData;
	HealingGunData *localGunData;
	double currentCharge;
	int nextTickTime;
	int lastUpdateTime;
	//hitPointsTargets:Array = [];
	//muzzlePosGlobal:Vector3 = new Vector3();
	//barrelOrigin:Vector3 = new Vector3();
	//axisX:Vector3 = new Vector3();
	//axisY:Vector3 = new Vector3();
	//vec:Vector3 = new Vector3();
	//pos3d:Vector3d = new Vector3d(0, 0, 0);

public:
	HealingGunModel();
	virtual ~HealingGunModel();

	//void stopEffects(TankData *tankData) override;
	//void reset() override;
	//void setLocalUser(TankData *tankData) override;
	//void clearLocalUser() override;
	//void activateWeapon(int time) override;
	//void deactivateWeapon(int time, bool sendServerCommand) override;
	//double update(int time, int deltaTime) override;


	void initObject(ClientObjectWeapon *clientObject,
					double angle,
					int capacity,
					int chargeRate,
					int checkPeriodMsec,
					double coneAngle,
					int dischargeRate,
					double radius);


	void init(ClientObjectWeapon *clientObject, QVector<IsisAction*> &isisActions);
	void startWeapon(ClientObjectUser *clientObject, const IsisAction *isisAction);
	void stopWeapon(ClientObjectUser *clientObject, const QString &healerId);
	void stopEffects(TankData *ownerTankData);
	void reset();
	void setLocalUser(TankData *localUserData);
	void clearLocalUser();
	void activateWeapon(int time);
	void deactivateWeapon(int time, bool sendServerCommand);

private:
	void stopWeaponCommand();
public:
	double update(int time, int deltaTime);
private:
	void actCommand(int victimInc, const QString &victimId, Vector3 *pos3d, int vLength);
	void actCommand();
public:
	void handleTankEvent(int eventType, TankData *tankData) override;


private:
	TankData* lockTarget(WeaponCommonData *commonData, TankData *weaponOwnerData);
	bool validateTarget();
	void startActiveMode(TankData *shooterData, TankData *targetData);
	void startWeaponCommand();
	void startWeaponCommand(int incId, const QString &victimId);
	TankData* getTankData(const QString &userId);
};

#endif // HEALINGGUNMODEL_H
