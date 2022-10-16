#ifndef TERMINATORMODEL_H
#define TERMINATORMODEL_H

#include "../IWeaponController.h"
#include <memory>
#include <QVector>

class ClientObject;
class ClientObjectUser;
class ClientObjectWeapon;
class Array;
class WeaponCommonData;
class ShotData;
class TerminatorData;
class BattlefieldModel;
class IWeaponCommonModel;
class ITank;
class TerminatorTargetSystem;
class TerminatorShotResult;
class TextureMaterial;
class Vector3;
class TerminatorItemHitUser;


class TerminatorModel : public IWeaponController
{
	BattlefieldModel *battlefieldModel;
	ITank *tankModel;
	IWeaponCommonModel *commonModel;
	TankData *localTankData;
	ShotData *localShotData;
	TerminatorData *localTerminatorData;
	WeaponCommonData *localWeaponCommonData;
	TerminatorTargetSystem *targetSystem;
	TerminatorShotResult *shotResult;

	double activatedTime;

	int chargeTimeLeft;
	int nextReadyTime;
	int shotBarrel;
	int defaultReloadMsec;

	bool firstshot;
	bool quickShot;
	bool lock;
	bool double_shoot;
	bool _triggerPressed;

	std::shared_ptr<TextureMaterial> decalMaterial;

	QVector<TerminatorItemHitUser*> *targets;

public:
	TerminatorModel();
	virtual ~TerminatorModel();

	void objectLoaded(ClientObjectWeapon *clientObject);
	void objectUnloaded(ClientObjectWeapon *clientObject);
	void initObject(ClientObjectWeapon *clientObject, int chargingTimeMsec, double weakeningCoeff);
	void startFire(ClientObjectUser *clientObject,int barrel);
	void fire(ClientObjectUser *clientObject, QVector<Vector3*> &affectedPoints, QStringList &affectedTankIds, bool doubleShot);
	void activateWeapon(int time);
	void deactivateWeapon(int time, bool sendServerCommand);
	void setLocalUser(TankData *localUserData);
	void clearLocalUser();
	double update(int time, int deltaTime);

private:
	void startFireCommand(int currBarrel);

public:
	void reset();
	void stopEffects(TankData *ownerTankData);

private:
	void doFire(WeaponCommonData *commonData, TankData *tankData, int time, bool _arg_4);
	void fireCommand(const Vector3 *hitPoint, int currBarrel, bool double_shoot);
	void fireCommand(QVector<TerminatorItemHitUser*> *targets, int length, const Vector3 *lastHitPoint, int currBarrel, bool double_shoot);
};

#endif // TERMINATORMODEL_H
