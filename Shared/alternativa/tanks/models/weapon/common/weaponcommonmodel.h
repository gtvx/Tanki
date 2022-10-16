#ifndef WEAPONCOMMONMODEL_H
#define WEAPONCOMMONMODEL_H

#include "IWeaponCommonModel.h"
#include "../IWeapon.h"

class IWeaponController;
class ClientObject;
class ClientObjectWeapon;
class WeaponCommonData;
class Tank;
class Vector3;
class Camera3D;
class TankData;
class TankModel;
class IBattleField;


class WeaponCommonModel : public IWeaponCommonModel, public IWeapon
{
	IWeaponController *localWeaponController;
	TankData *localUserData;
	WeaponCommonData *localCommonData;
	TankModel *tankModel;
	//int numObjects;
	IBattleField *battlefield;

public:

	bool enabled;
	bool triggerKeyDown;
	bool triggerKeyUp;
	bool triggerPressed;
	double indicatorValue;


	WeaponCommonModel();
	virtual ~WeaponCommonModel();

	IWeaponController *getWeaponController() override;
	bool initObject(ClientObjectWeapon *clientObject, double impactForce, double kickback, double turretRotationAcceleration, double turretRotationSpeed);
	void objectLoaded(ClientObjectWeapon *clientObject);
	WeaponCommonData *getCommonData(ClientObjectWeapon *clientObject) override;

	void ownerLoaded(ClientObjectUser*) override;
	void ownerUnloaded(ClientObjectUser*) override;
	void ownerDisabled(ClientObjectUser*) override;
	double update(int time, int deltaTime) override;
	void reset() override;
	void enable() override;
	void disable() override;
	void stop() override;
	double getTurretRotationAccel(ClientObjectWeapon*) override;
	double getTurretRotationSpeed(ClientObjectWeapon*) override;
	void createShotEffects(ClientObjectWeapon *turretObject, Tank *firingTank, int barrelIndex, const Vector3 *globalMuzzlePosition, const Vector3 *globalGunDirection) override;
	void createExplosionEffects(ClientObjectWeapon *turretObject, Camera3D *camera, bool isLocalHitPosition, const Vector3 *hitPosition, const Vector3 *globalHitDirection, TankData *victimData, double impactCoeff) override;
	void onKey(int code, bool down);
};


#endif // WEAPONCOMMONMODEL_H
