#ifndef SMOKYMODEL_H
#define SMOKYMODEL_H

#include "../IWeaponController.h"
#include "alternativa/math/vector3.h"
#include <memory>

//public class SmokyModel extends GunModelBase implements IGunModelBase, IObjectLoadListener, IWeaponController

class ClientObjectWeapon;
class TankModel;
class WeaponCommonModel;
class ClientObject;
class ClientObjectUser;
class QString;
class Vector3;
class BattlefieldModel;
class WeaponCommonData;
class ShotData;
class CommonTargetEvaluator;
class CommonTargetSystem;
class CommonTargetSystemBot;
class TextureMaterial;


class SmokyModel : public IWeaponController
{
	TankModel *tankModel;
	WeaponCommonModel *weaponCommonModel;
	TankData *localTankData;
	BattlefieldModel *battlefieldModel;
	ShotData *localShotData;
	CommonTargetEvaluator *targetEvaluator;
	double maxTargetingDistance;
	int currTime;
	Vector3 muzzle;
	CommonTargetSystem *targetSystem;
	std::shared_ptr<TextureMaterial> decalMaterial;
	Vector3 *barrelOrigin;

public:
	bool _triggerPressed;
	int nextReadyTime;
	CommonTargetSystemBot *targetSystemBot;
	WeaponCommonData *localWeaponCommonData;

	SmokyModel();
	virtual ~SmokyModel();

	void objectLoaded(ClientObjectWeapon *clientObject);

	void stopEffects(TankData *tankData) override;
	void reset() override;
	void setLocalUser(TankData *tankData) override;
	void clearLocalUser() override;
	void activateWeapon(int time) override;
	void deactivateWeapon(int time, bool sendServerCommand) override;
	double update(int time, int deltaTime) override;
	void fire(ClientObjectUser *user, const Vector3 &affectedPoint, const QString &affectedTankId, double weakeningCoeff);
private:
	void fireCommand(ClientObjectWeapon *turret, double distance, const Vector3 *hitPos, const QString &victimId, int victimInc, const Vector3 *tankPos);
};

#endif // SMOKYMODEL_H
