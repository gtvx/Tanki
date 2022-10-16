#ifndef RAILGUNMODEL_H
#define RAILGUNMODEL_H

#include "../IWeaponController.h"
#include <QVector>
#include "memory"

class ClientObject;
class Array;
class QString;
class RailgunTargetSystem;
class RailgunShotResult;
class ITank;
class IWeaponCommonModel;
class ShotData;
class RailgunData;
class WeaponCommonData;
class BattlefieldModel;
class ClientObjectWeapon;
class ClientObjectUser;
class Vector3;
class TextureMaterial;
class RailgunItemHitUser;


class RailgunModel : public IWeaponController
{
	BattlefieldModel *battlefieldModel;
	ITank *tankModel;
	IWeaponCommonModel *commonModel;
	TankData *localTankData;
	ShotData *localShotData;
	RailgunData *localRailgunData;
	WeaponCommonData *localWeaponCommonData;
	bool _triggerPressed;
	int chargeTimeLeft;
	int nextReadyTime;
	RailgunTargetSystem *targetSystem;
	RailgunShotResult *shotResult;
	bool firstshot;
	std::shared_ptr<TextureMaterial> decalMaterial;
	QVector<RailgunItemHitUser*> *targets;

public:
	RailgunModel();
	virtual ~RailgunModel();

	void objectLoaded(ClientObjectWeapon *clientObject);
	void objectUnloaded(ClientObjectWeapon *clientObject);
	void initObject(ClientObjectWeapon *clientObject, int chargingTimeMsec, double weakeningCoeff);
	void startFire(ClientObjectUser *clientObject, const QString &firingTankId);
	void fire(ClientObjectUser *clientObject, QVector<Vector3*> &affectedPoints, QStringList &affectedTankIds);
	void activateWeapon(int time);
	void deactivateWeapon(int time, bool sendServerCommand);
	void setLocalUser(TankData *localUserData);
	void clearLocalUser();
	double update(int time, int deltaTime);
	void startFireCommand();
	void reset();
	void stopEffects(TankData *ownerTankData);

private:
	void doFire(WeaponCommonData *commonData, TankData *tankData, int time);
	void fireCommand(const Vector3 *hitPoints);
	void fireCommand(QVector<RailgunItemHitUser*> *targets, int length, const Vector3 *lastHitPoint);
};

#endif // RAILGUNMODEL_H
