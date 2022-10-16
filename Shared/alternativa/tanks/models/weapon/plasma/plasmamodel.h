#ifndef PLASMAMODEL_H
#define PLASMAMODEL_H

#include <QString>
#include <QHash>
#include "alternativa/math/vector3.h"
#include "IPlasmaShotListener.h"
#include "../IWeaponController.h"
#include <memory>

class ClientObject;
class Vector3;
class TankData;
class PlasmaShot;
class Body;
class PlasmaGunData;
class ClientObjectWeapon;
class ShotData;
class WeaponCommonData;
class IBattleField;
class TankModel;
class WeaponCommonModel;
class IWeaponWeakeningModel;
class CommonTargetSystem;
class CommonTargetEvaluator;
class TextureMaterial;
class ClientObjectUser;


//extends PlasmaModelBase implements IPlasmaModelBase, IObjectLoadListener, IPlasmaShotListener, IWeaponController

class PlasmaModel : public IPlasmaShotListener, public IWeaponController
{

	bool _triggerPressed;
	int shotId;
	QHash<QString, QHash<int, PlasmaShot*>*> activeShots;
	//activeShots:Dictionary = new Dictionary();
	TankData *localTankData;
	ShotData *localShotData;
	WeaponCommonData *localWeaponCommonData;
	//IModelService *modelService;
	IBattleField *battlefieldModel;
	TankModel *tankModel;
	WeaponCommonModel *weaponCommonModel;
	IWeaponWeakeningModel *weaponWeakeningModel;
	CommonTargetSystem *targetSystem;
	//HitInfo *hitInfo = new HitInfo();
	//weaponUtils:WeaponUtils = WeaponUtils.getInstance();
	int nextReadyTime;
	int currentTime;
	Vector3 _barrelOrigin;
	//_dirToTarget:Vector3 = new Vector3();
	//_barrelOrigin:Vector3 = new Vector3();
	//_xAxis:Vector3 = new Vector3();
	//_hitPosGlobal:Vector3 = new Vector3();
	//_hitPosLocal:Vector3 = new Vector3();
	//_muzzlePosGlobal:Vector3 = new Vector3();
	//_hitPos:Vector3 = new Vector3();
	//_hitPos3d:Vector3d = new Vector3d(0, 0, 0);
	//_tankPos3d:Vector3d = new Vector3d(0, 0, 0);
	//_dirToTarget3d:Vector3d = new Vector3d(0, 0, 0);
	//intersection:RayIntersection = new RayIntersection();
	Vector3 _gunDirGlobal;
	CommonTargetEvaluator *targetEvaluator;
	double maxTargetingDistance;

	std::shared_ptr<TextureMaterial> decalMaterial;
public:

	PlasmaModel();
	virtual ~PlasmaModel();

	void objectLoaded(ClientObject *clientObject);
	void objectUnloaded(ClientObject *clientObject);
	void initObject(ClientObjectWeapon *clientObject, double chargeRadius, double distance, double speed);
	void fire(ClientObjectUser *clientObject, const QString &firingTankId, int barrel, int shotId, const Vector3 *dirToTarget);
	void hit(ClientObjectUser *clientObject, const QString &firingTankId, int shotId, const Vector3 *affectedPoint, const QString &affectedTankId, double weakeningCoeff);
	void setLocalUser(TankData *localUserData) override;
	void clearLocalUser();
	double update(int time, int deltaTime) override;

private:
	void fireCommand(int currBarrel, int realShotId, const Vector3 *_dirToTarget3d);

public:
	void activateWeapon(int time) override;
	void deactivateWeapon(int time, bool sendServerCommand) override;
	void reset() override;
	void stopEffects(TankData *tankData) override;
	void plasmaShotDissolved(PlasmaShot *shot) override;
	void plasmaShotHit(PlasmaShot *shot, const Vector3 *hitPoint, const Vector3 *hitDir, Body *body) override;

private:
	void hitCommand(int shotId, const Vector3 *hitPos, int distance);
	void hitCommand(int shotId, const Vector3 *hitPos, const QString &affectedTankId, int incr, const Vector3 *tankPos, int distance);
	PlasmaGunData* getWeaponData(ClientObjectWeapon *clientObject);
	void removeShot(PlasmaShot *shot);
	void createShot(bool active, int shotId, TankData *tankData, const Vector3 *muzzleGlobalPos, const Vector3 *dirToTarget);
	void cacheInterfaces();
};

#endif // PLASMAMODEL_H
