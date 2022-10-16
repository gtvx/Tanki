#ifndef FLAMETHROWERMODEL_H
#define FLAMETHROWERMODEL_H

#include "../IWeaponController.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/models/weapon/flamethrower/IFlamethrower.h"
#include <qglobal.h>
#include <QHash>


class ClientObjectWeapon;
class ClientObjectUser;
class FlamethrowerData;
class WeaponCommonData;
class ClientObjectWeaponFlamethrower;
class QString;
class ConicAreaTargetSystem;
class ConicAreaTargetSystemBot;
class BattlefieldModel;
class TankModel;
class WeaponCommonModel;
class ITargetValidator;
class Vector3;
class FlamethrowerItemHitUser;
class IWeaponWeakeningModel;
class EffectsPair;


class FlamethrowerModel : public IWeaponController, public IFlamethrower
{
	ConicAreaTargetSystem *targetSystem;
	TankData *localTankData;
	BattlefieldModel *battlefield;
	TankModel *tankInterface;
	WeaponCommonModel *commonModel;
	FlamethrowerData *localFlameData;
	ITargetValidator *targetValidator;
	QVector<FlamethrowerItemHitUser*> *targets;
	Vector3 muzzle;
	IWeaponWeakeningModel *weakeningModel;
	QHash<QString, EffectsPair*> activeEffects;

public:
	ConicAreaTargetSystemBot *targetSystemBot;
	WeaponCommonData *localWeaponCommonData;

	int lastUpdateTime;
	int nextTargetCheckTime;
	bool active;
	double currentHeat;

	FlamethrowerModel();
	virtual ~FlamethrowerModel();

	void initObject(ClientObjectWeapon *clientObject, double coneAngle, int coolingSpeed, int heatingLimit, int heatingSpeed, double range, int targetDetectionIntervalMsec);
	void startFire(ClientObjectUser *clientObject, const QString &firingTankId);
	void stopFire(ClientObjectUser *clientObject, const QString &firingTankId);
	void setLocalUser(TankData *localUserData);
	void clearLocalUser();
	double update(int time, int deltaTime);
	void startFireCommand();
	void stopFireCommand();

public:
	void activateWeapon(int time);
	void reset();
	void stopEffects(TankData *ownerTankData);
	void deactivateWeapon(int time, bool sendServerCommand);

	FlamethrowerData *getFlameData(ClientObjectWeapon *clientObject) override;
private:
	void addFlameEffects(WeaponCommonData *commonData, TankData *tankData);
	void removeFlameEffects(const QString &userId);
	void hitCommand(QVector<FlamethrowerItemHitUser*> *targets,  int len);
};

#endif // FLAMETHROWERMODEL_H
