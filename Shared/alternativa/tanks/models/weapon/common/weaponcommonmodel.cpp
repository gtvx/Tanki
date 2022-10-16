#include "weaponcommonmodel.h"
#include "weaponcommondata.h"
#include "../WeaponConst.h"
#include "alternativa/math/vector3.h"
#include "resource/resourceutil.h"
#include "alternativa/object/clientobject.h"
#include "resource/tanks/tankresource.h"
#include "alternativa/object/clientobjectweapon.h"
#include "battlecontroller.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/physics/bodystate.h"
#include "../IWeaponController.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/sfx/shoot/ICommonShootSFX.h"
#include "alternativa/tanks/sfx/effectspair.h"
#include "tanks/weaponsmanager.h"
#include "service.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "gametime.h"
#include <QVector>





WeaponCommonModel::WeaponCommonModel()
{
	_function_name("WeaponCommonModel::WeaponCommonModel");
	localWeaponController = nullptr;
	//numObjects = 0;
	enabled = false;
	triggerKeyDown = false;
	triggerKeyUp = false;
	triggerPressed = false;
	indicatorValue = 0;
	localUserData = nullptr;
	localCommonData = nullptr;
	tankModel = nullptr;
	this->battlefield = nullptr;
}

WeaponCommonModel::~WeaponCommonModel()
{

}

IWeaponController* WeaponCommonModel::getWeaponController()
{
	return this->localWeaponController;
}

bool WeaponCommonModel::initObject(ClientObjectWeapon *clientObject, double impactForce, double kickback, double turretRotationAcceleration, double turretRotationSpeed)
{
	_function_name("WeaponCommonModel::initObject");

	if (clientObject == nullptr)
		return false;

	//if (this->modelService == nullptr)
	//{
	//this->modelService = (Main.osgi.getService(IModelService) as IModelService);
	//this->battlefield = (Main.osgi.getService(IBattleField) as IBattleField);
	//this->tankModel = (Main.osgi.getService(ITank) as ITank);
	//}

	this->battlefield = Service::getIBattleField();

	tankModel = Service::getTankModel();

	if (clientObject->weaponCommonData == nullptr)
	{
		std::shared_ptr<WeaponCommonData> weaponData = std::make_shared<WeaponCommonData>();
		double correctionCoeff = WeaponConst::BASE_IMPACT_FORCE;
		weaponData->kickback = kickback * correctionCoeff;
		weaponData->impactCoeff = impactForce;
		weaponData->impactForce = impactForce * correctionCoeff;
		weaponData->turretRotationAccel = turretRotationAcceleration;
		weaponData->turretRotationSpeed = turretRotationSpeed;
		TankResource *resource_muzzles = ResourceUtil::getResourceModel(clientObject->getId());
		if (resource_muzzles == nullptr)
			return false;
		weaponData->setMuzzles(resource_muzzles->muzzles);
		clientObject->weaponCommonData = weaponData;
	}

	return true;
}



void WeaponCommonModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	_function_name("WeaponCommonModel::objectLoaded");
	WeaponCommonData *weaponData = this->getCommonData(clientObject);
	if (weaponData != nullptr)
		weaponData->weaponController = Service::getBattleController()->getWeaponController(clientObject);
}

WeaponCommonData* WeaponCommonModel::getCommonData(ClientObjectWeapon *clientObject)
{
	return clientObject->weaponCommonData.get();
}

void WeaponCommonModel::ownerLoaded(ClientObjectUser *ownerObject)
{
	_function_name("WeaponCommonModel::ownerLoaded");
	TankData *ownerTankData = ownerObject->tankData;
	if (ownerTankData->local == true)
	{
		this->indicatorValue = 0;
		this->localUserData = ownerTankData;
		this->localCommonData = this->getCommonData(this->localUserData->turret);
		this->localWeaponController = this->localCommonData->weaponController;

		//if (this->localWeaponController == nullptr)
			//this->localWeaponController = BattleController::getWeaponController(ownerObject);

		if (this->localWeaponController != nullptr)
			this->localWeaponController->setLocalUser(ownerTankData);

		//this->addKeyboardListeners();
		this->triggerKeyDown = this->triggerKeyUp = false;
		this->enabled = false;
	}
}

void WeaponCommonModel::ownerUnloaded(ClientObjectUser *ownerObject)
{
	_function_name("WeaponCommonModel::ownerUnloaded");

	TankData *ownerTankData = this->tankModel->getTankData(ownerObject);
	WeaponCommonData *commonData = this->getCommonData(ownerTankData->turret);
	if (commonData->weaponController != nullptr)
	{
		commonData->weaponController->stopEffects(ownerTankData);
	}

	if (ownerTankData->local == true)
	{
		if (this->localWeaponController != nullptr)
		{
			this->localWeaponController->deactivateWeapon(GetGameTime(), false);
			this->localWeaponController->clearLocalUser();
		}
		this->enabled = false;
		this->localUserData = nullptr;
		this->localCommonData = nullptr;
		this->localWeaponController = nullptr;
		//this->removeKeyboardListeners();
	}
}

void WeaponCommonModel::ownerDisabled(ClientObjectUser *ownerObject)
{
	_function_name("WeaponCommonModel::ownerDisabled");

	TankData* ownerTankData = this->tankModel->getTankData(ownerObject);
	WeaponCommonData *commonData = this->getCommonData(ownerTankData->turret);

	if (commonData->weaponController != nullptr)
	{
		commonData->weaponController->stopEffects(ownerTankData);
	}

	if (ownerTankData->local)
	{
		if (this->localWeaponController != nullptr)
		{
			this->localWeaponController->deactivateWeapon(GetGameTime(), false);
			this->localWeaponController->reset();
		}

		if (this->triggerKeyDown && this->triggerKeyUp)
		{
			this->triggerKeyDown = this->triggerKeyUp = false;
		}

		this->enabled = false;
		this->triggerPressed = false;
		this->indicatorValue = 0;
	}
}

double WeaponCommonModel::update(int time, int deltaTime)
{
	_function_name("WeaponCommonModel::update");

	if (this->localWeaponController == nullptr)
		return 0.;

	bool activatedThisUpdate = false;
	bool doUpdate = true;

	if (this->enabled)
	{
		if (this->triggerKeyDown)
		{
			if ((!(this->triggerPressed)))
			{
				this->triggerPressed = true;
				activatedThisUpdate = true;
				this->localWeaponController->activateWeapon(time);
			}
		}

		if (this->triggerKeyUp)
		{
			if (this->triggerPressed)
			{
				if (activatedThisUpdate)
				{
					this->indicatorValue = this->localWeaponController->update(time, deltaTime);
					doUpdate = false;
				}
				this->localWeaponController->deactivateWeapon(time, true);
				this->triggerPressed = false;
			}
			this->triggerKeyDown = (this->triggerKeyUp = false);
		}
	}

	if (doUpdate)
	{
		this->indicatorValue = this->localWeaponController->update(time, deltaTime);
	}


	return this->indicatorValue;
}

void WeaponCommonModel::reset()
{
	_function_name("WeaponCommonModel::reset");
	if (this->localWeaponController != nullptr)
		this->localWeaponController->reset();
}

void WeaponCommonModel::enable()
{
	_function_name("WeaponCommonModel::enable");
	if ((!(this->enabled)) && (!(this->localWeaponController == nullptr)))
		this->enabled = true;
}

void WeaponCommonModel::disable()
{
	_function_name("WeaponCommonModel::disable");

	if (((!(this->enabled)) || (this->localWeaponController == nullptr)))
		return;

	this->enabled = false;
	this->triggerKeyDown = false;
	this->triggerKeyUp = false;
	this->triggerPressed = false;
	this->localWeaponController->deactivateWeapon(GetGameTime(), true);
}

void WeaponCommonModel::stop()
{
	_function_name("WeaponCommonModel::stop");
	if (((!(this->enabled)) || (this->localWeaponController == nullptr)))
		return;

	this->triggerKeyUp = true;
}

double WeaponCommonModel::getTurretRotationAccel(ClientObjectWeapon *clientObject)
{
	 return this->getCommonData(clientObject)->turretRotationAccel;
}

double WeaponCommonModel::getTurretRotationSpeed(ClientObjectWeapon *clientObject)
{
	return this->getCommonData(clientObject)->turretRotationSpeed;
}



void WeaponCommonModel::createShotEffects(ClientObjectWeapon *turretObject, Tank *firingTank, int barrelIndex, const Vector3 *globalMuzzlePosition, const Vector3 *globalGunDirection)
{
	(void)barrelIndex;

	WeaponCommonData *commonData = this->getCommonData(turretObject);
	firingTank->addWorldForceScaled(globalMuzzlePosition, globalGunDirection, -commonData->kickback);
	ICommonShootSFX *sfxModel = WeaponsManager::getCommonShotSFX(WeaponsManager::getObjectFor(turretObject->getId()));
	if (sfxModel == nullptr)
		return;

	//if (this->battlefield == null)
	//this->battlefield = (Main.osgi.getService(IBattleField) as IBattleField);

	EffectsPair effectsPair;
	sfxModel->createShotEffects(turretObject,
								commonData->getMuzzle(barrelIndex),
								firingTank->skin->turretMesh,
								this->battlefield->getBattlefieldData()->viewport->camera,
								&effectsPair);

	this->battlefield->addGraphicEffect(effectsPair.graphicEffect);
	//this->battlefield->addSound3DEffect(effectsPair->soundEffect);
}



void WeaponCommonModel::createExplosionEffects(ClientObjectWeapon *turretObject,
											   Camera3D *camera,
											   bool isLocalHitPosition,
											   const Vector3 *hitPosition,
											   const Vector3 *globalHitDirection,
											   TankData *victimData,
											   double impactCoeff)
{
#ifdef GRAPHICS
	ICommonShootSFX *sfxModel = WeaponsManager::getCommonShotSFX(turretObject);
	if (sfxModel == nullptr)
		return;
#else
	(void)camera;
#endif


	Vector3 _hitPosition;
	if (victimData != nullptr)
	{
		if (isLocalHitPosition)
		{
			victimData->tank->baseMatrix.transformVector(hitPosition, &_hitPosition);
			_hitPosition.add(&victimData->tank->state.pos);
		}
		else
			_hitPosition.copy(hitPosition);

		victimData->tank->addWorldForceScaled(&_hitPosition, globalHitDirection, this->getCommonData(turretObject)->impactForce * impactCoeff);
	}
	else
	{
		_hitPosition.copy(hitPosition);
	}

#ifdef GRAPHICS
	EffectsPair effectsPair;
	sfxModel->createExplosionEffects(turretObject, &_hitPosition, camera, impactCoeff, &effectsPair);
	//if (this->battlefield == null)
	//this->battlefield = (Main.osgi.getService(IBattleField) as IBattleField);
	this->battlefield->addGraphicEffect(effectsPair.graphicEffect);
	//this->battlefield.addSound3DEffect(effectsPair.soundEffect);
#endif
}


void WeaponCommonModel::onKey(int code, bool down)
{
	if (code != 32)
		return;

	if (down == true)
	{
		this->triggerKeyDown = true;
		this->triggerKeyUp = false;
	}
	else
	{
		if (this->enabled == false)
		{
			this->triggerKeyDown = this->triggerKeyUp = false;
		}
		else
		{
			this->triggerKeyUp = true;
		}
	}
}
