#ifndef SHAFTMODEL_H
#define SHAFTMODEL_H

#include "../IWeaponController.h"
#include "alternativa/engine3d/core/Object3DFilter.h"
#include <memory>
#include <QVector>
#include <QHash>

class ClientObject;
enum class ShaftEnergyMode;
class ShaftShotResult;
class Vector3;
class Array;
class AimedShotResult;
class Object3D;
class Tank;
class TankSkin;
class TargetingController;
class ShaftData;
class BattlefieldModel;
class ClientObjectWeapon;
class AllGlobalGunParams;
class IWeaponCommonModel;
class ShaftTargetSystem;
class ClientObjectUser;
class TankModel;
class ServerShaftTargetData;
class Aim;
class ShotData;
class WeaponCommonData;
class SetControllerForTemporaryItems;
class TextureMaterial;
class ImageItem;



class ShaftModel : public IWeaponController
{
	bool activate;
	bool updateController;
	bool lock;
	bool lockCheckIntersection;
	bool fired;
	bool quickShot;
	bool aim;

	int energyBaseTime;
	int exitTime;
	int indicatorIndexFrame;

	double activatedTime;
	double energyAdditive;
	double energySpeed;

	AllGlobalGunParams *allGunParams;
	Tank *tank;
	TankData *localTankData;
	ShotData *localShotData;
	WeaponCommonData *localWeaponCommonData;
	BattlefieldModel *battlefieldModel;
	TankModel *tankModel;
	IWeaponCommonModel *weaponCommonModel;
	TargetingController *targetController;
	ShaftEnergyMode energyMode;
	QHash<Object3D*, bool> exclusionSet;
	SetControllerForTemporaryItems *exclusionSetController;
	ShaftTargetSystem *quickShotTargetSystem;
	ShaftShotResult *shotResult;
	IWeaponCommonModel *commonModel;
	std::shared_ptr<TextureMaterial> decalMaterial;
	ImageItem *image;
	ImageItem *indicatorBitmap;


	class Filter : public Object3DFilter
	{
	public:
		QHash<Object3D*, bool> *exclusionSet;

		bool check(Object3D*) override;
	} exclusionSetFilter;


public:

	ShaftData *shaftData;

	ShaftModel();
	virtual ~ShaftModel();

	void initObject(ClientObjectWeapon *clientObject,
					double maxEnegry,
					double chargeRate,
					double dischargeRate,
					double elevationAngleUp,
					double elevationAngleDown,
					double verticalTargetingSpeed,
					double horizontalTargetingSpeed,
					double initialFOV,
					double minimumFOV,
					double shrubsHidingRadiusMin,
					double shrubsHidingRadiusMax,
					double impactQuickShot);

	void objectLoaded(ClientObject *object);
	void setEnergyMode(ShaftEnergyMode mode);
	void objectUnloaded(ClientObject *object);


public:
	double update(int param1, int param2);

private:
	void forceExit(int time);
	void sendQuickShotCommand(ShaftShotResult *result);
	void sendStartAimCommand();
	void sendStopAimCommand(int physTime, Vector3 *staticShot, QVector<Aim*> *aims);
	void onQuickShot();
public:

	void quickFire(ClientObjectUser *user, const QStringList &targets, QVector<Vector3*> *hitPoints, const Vector3 *dir);
	void fire(ClientObjectUser *user, const Vector3 *staticHit, QVector<ServerShaftTargetData*> *targets);

	void performAimedShot(double);

private:
	void createTankShotEffect(ClientObjectUser *user, const Vector3 *point, Vector3 *dir);
	void createStaticShotEffect(ClientObjectUser *user, const Vector3 *point, Vector3 *dir);
	bool hasIntersection();
	AimedShotResult* getAimedShotTargets();
	bool isValidHit(Tank *param1, Object3D *param2, Vector3 *param3);
	void addTankSkinToExclusionSet(TankSkin *tankSkin);

public:
	void targetModeOn();
	double getStatus();
	void doSetEnergyMode(ShaftEnergyMode mode, double energy, int physTime);
	void sendBeginEnergyDrain(int time);
	double getEnergy(double energyTime);

private:
	void updateCrossPosition();
	void getShotDirection(Vector3 *param1, Vector3 *param2, Vector3 *param3, Vector3 *result);

public:
	void stopEffects(TankData *ownerTankData);
	void reset();
	void setLocalUser(TankData *localUserData);
	void clearLocalUser();
	void activateWeapon(int time);
	void deactivateWeapon(int time, bool sendServerCommand);
	TargetingController* getTargetingController();
};

#endif // SHAFTMODEL_H
