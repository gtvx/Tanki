#include "smokymodel.h"
#include "service.h"
#include "alternativa/math/vector3.h"
#include "battlecontroller.h"
#include <QHash>
#include <QHashIterator>
#include "_global.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/common/weaponcommonmodel.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/object/clientobjectweapon.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/tanks/models/weapon/common/HitInfo.h"
#include "alternativa/tanks/models/weapon/shared/commontargetsystem.h"
#include "alternativa/tanks/models/weapon/shared/commontargetsystembot.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/tanks/models/weapon/weakening/weaponweakeningmodel.h"
#include "alternativa/tanks/models/weapon/shared/commontargetevaluator.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "networking/network.h"
#include "double_to_string.h"
#include "resource/resourceutil.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/models/battlefield/decals/RotationState.h"


SmokyModel::SmokyModel()
{
	_function_name("SmokyModel::SmokyModel");
	tankModel = nullptr;
	weaponCommonModel = nullptr;
	localTankData = nullptr;
	battlefieldModel = nullptr;
	localWeaponCommonData = nullptr;
	localShotData = nullptr;
	currTime = 0;
	nextReadyTime = 0;
	maxTargetingDistance = 100000.;
	targetEvaluator = nullptr;
	targetSystem = nullptr;
	targetSystemBot = nullptr;
	//_interfaces.push(IModel, IGunModelBase, IObjectLoadListener, IWeaponController);
#ifdef GRAPHICS
	decalMaterial = new_TextureMaterial();
	decalMaterial->init();
	decalMaterial->setTexture(ResourceUtil::getResourceImageLocal("decal/smoky.png"));
#endif
	barrelOrigin = new Vector3(0, 0, 0);
}

SmokyModel::~SmokyModel()
{
	delete barrelOrigin;
}

void SmokyModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	_function_name("SmokyModel::objectLoaded");
	(void)clientObject;

	this->battlefieldModel = Service::getBattlefieldModel();
	this->tankModel = Service::getTankModel();
	this->weaponCommonModel = Service::getWeaponCommonModel();
	//this->weaponWeakeningModel = IWeaponWeakeningModel(this->modelService.getModelsByInterface(IWeaponWeakeningModel)[0]);
	//(this->battlefieldModel as BattlefieldModel).blacklist.push(decalMaterial.getTextureResource());
}



void SmokyModel::fire(ClientObjectUser *user, const Vector3 &affectedPoint, const QString &affectedTankId, double weakeningCoeff)
{
	_function_name("SmokyModel::fire");

	ClientObjectUser *firingTank = user;
	if (firingTank == nullptr)
		return;

	if (this->tankModel->localUserData != nullptr)
		if (firingTank == this->tankModel->localUserData->user)
			return;

	TankData *firingTankData = this->tankModel->getTankData(firingTank);
	WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(firingTankData->turret);
	int barrelIndex = 0;
	Vector3 gunDirGlobal;
	Vector3 muzzlePosGlobal;

	const Vector3 *muzzle = commonData->getMuzzle(barrelIndex);
	WeaponUtils::calculateGunParamsAux(firingTankData->tank->skin->turretMesh, muzzle, &muzzlePosGlobal, &gunDirGlobal);
	this->weaponCommonModel->createShotEffects(firingTankData->turret, firingTankData->tank, barrelIndex, &muzzlePosGlobal, &gunDirGlobal);
	//if ((((affectedPoint == nullptr) || (isNaN(affectedPoint.x))) || (isNaN(affectedPoint.y))))
	//return;


	Vector3 hitPos;
	hitPos.x = affectedPoint.x;
	hitPos.y = affectedPoint.y;
	hitPos.z = affectedPoint.z;
	if (affectedTankId.isEmpty() == false)
	{
		ClientObjectUser *affectedTankObject = Service::getBattleController()->tanks->value(affectedTankId);
		if (affectedTankObject == nullptr)
			return;

		TankData *affectedTankData = this->tankModel->getTankData(affectedTankObject);
		if (affectedTankData == nullptr || affectedTankData->tank == nullptr)
			return;

		Camera3D *camera = this->battlefieldModel->getBattlefieldData()->viewport->camera;
		this->weaponCommonModel->createExplosionEffects(firingTankData->turret, camera, true, &hitPos, &gunDirGlobal, affectedTankData, weakeningCoeff);
		//this->battlefieldModel->tankHit(affectedTankData, this->_gunDirGlobal, weakeningCoeff * commonData.impactCoeff);
	}
	else
	{
		Camera3D *camera = this->battlefieldModel->getBattlefieldData()->viewport->camera;
		this->weaponCommonModel->createExplosionEffects(firingTankData->turret, camera, false, &hitPos, &gunDirGlobal, nullptr, weakeningCoeff * commonData->impactCoeff);
		//this->weaponCommonModel->createExplosionEffects(firingTankData.turret, this->battlefieldModel.getBattlefieldData().viewport.camera, false, affectedPoint.toVector3(), this->_gunDirGlobal, null, (weakeningCoeff * commonData.impactCoeff));
		this->battlefieldModel->addDecal(&affectedPoint, this->barrelOrigin, 250, decalMaterial.get(), RotationState::NONE);
	}
}


void SmokyModel::stopEffects(TankData *tankData)
{
	_function_name("SmokyModel::stopEffects");
	(void)tankData;
}

void SmokyModel::reset()
{
	_function_name("SmokyModel::reset");
	this->_triggerPressed = false;
	this->nextReadyTime = 0;
}

void SmokyModel::setLocalUser(TankData *tankData)
{
	_function_name("SmokyModel::setLocalUser");
	this->objectLoaded(nullptr);
	this->localTankData = tankData;
	this->localShotData = WeaponsManager::shotData(tankData->turret->getId());
	this->localWeaponCommonData = this->weaponCommonModel->getCommonData(tankData->turret);
	this->muzzle.copy(localWeaponCommonData->getMuzzle(0));

	this->targetEvaluator = CommonTargetEvaluator::create(this->localTankData,
														  this->localShotData,
														  this->battlefieldModel,
														  true, nullptr);

	this->targetSystem = new CommonTargetSystem(this->maxTargetingDistance,
												this->localShotData->autoAimingAngleUp,
												this->localShotData->numRaysUp,
												this->localShotData->autoAimingAngleDown,
												this->localShotData->numRaysDown,
												(ICollisionDetector*)this->battlefieldModel->getBattlefieldData()->collisionDetector,
												this->targetEvaluator);


	this->targetSystemBot = new CommonTargetSystemBot(this->maxTargetingDistance,
													  this->localShotData->autoAimingAngleUp,
													  this->localShotData->numRaysUp,
													  this->localShotData->autoAimingAngleDown,
													  this->localShotData->numRaysDown,
													  (ICollisionDetector*)this->battlefieldModel->getBattlefieldData()->collisionDetector,
													  this->targetEvaluator);

	this->reset();
}

void SmokyModel::clearLocalUser()
{
	_function_name("SmokyModel::clearLocalUser");
	if (this->targetSystem != nullptr)
		delete this->targetSystem;
	if (this->targetSystemBot != nullptr)
		delete this->targetSystemBot;
	if (this->targetEvaluator != nullptr)
		delete this->targetEvaluator;

	this->localTankData = nullptr;
	this->localShotData = nullptr;
	this->localWeaponCommonData = nullptr;
	this->targetSystem = nullptr;
	this->targetEvaluator = nullptr;
}

void SmokyModel::activateWeapon(int time)
{
	_function_name("SmokyModel::activateWeapon");
	(void)time;
	_triggerPressed = true;
}

void SmokyModel::deactivateWeapon(int time, bool sendServerCommand)
{
	_function_name("SmokyModel::deactivateWeapon");
	(void)time;
	(void)sendServerCommand;
	_triggerPressed = false;
}

double SmokyModel::update(int time, int deltaTime)
{
	_function_name("SmokyModel::update");
	(void)deltaTime;

	this->currTime = time;
	if (((!(this->_triggerPressed)) || (this->currTime < this->nextReadyTime)))
	{
		if (this->currTime < this->nextReadyTime)
		{
			return 1. + (double(this->currTime - this->nextReadyTime) / this->localShotData->reloadMsec);
		}
		return 1.;
	}


	Vector3 gunDirGlobal;
	Vector3 xAxis;
	Vector3 muzzlePosGlobal;

	this->nextReadyTime = this->currTime + this->localShotData->reloadMsec;
	WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh, &muzzle, &muzzlePosGlobal, this->barrelOrigin, &xAxis, &gunDirGlobal);
	this->weaponCommonModel->createShotEffects(this->localTankData->turret, this->localTankData->tank, this->localWeaponCommonData->currBarrel, &muzzlePosGlobal, &gunDirGlobal);
	BattlefieldData *bfData = this->battlefieldModel->getBattlefieldData();
	TankData *victimData = nullptr;

	Vector3 _hitPos;
	Vector3 *hitPos = nullptr;

	HitInfo hitInfo;

	double distance = 0;
	if (this->targetSystem->getTarget(this->barrelOrigin, &gunDirGlobal, &xAxis, this->localTankData->tank, &hitInfo))
	{
		distance = hitInfo.distance * 0.01;

		hitPos = &_hitPos;

		hitPos->x = hitInfo.position.x;
		hitPos->y = hitInfo.position.y;
		hitPos->z = hitInfo.position.z;

		if (hitInfo.body != nullptr)
		{
			QHashIterator<TankData*, bool> i(*bfData->activeTanks);
			while (i.hasNext())
			{
				i.next();
				TankData *tankdata = i.key();
				if (hitInfo.body == tankdata->tank)
				{
					victimData = tankdata;
					break;
				}
			}
		}

		double impactCoeff = WeaponWeakeningModel::getImpactCoeff(this->localTankData->turret, distance);
		this->weaponCommonModel->createExplosionEffects(this->localTankData->turret, bfData->viewport->camera, false, &hitInfo.position, &gunDirGlobal, victimData, impactCoeff);
		if (victimData != nullptr)
		{
			Vector3 hitPosGlobal, hitPosLocal;
			hitPosGlobal.diff(&hitInfo.position, &victimData->tank->state.pos);
			victimData->tank->baseMatrix.transformVectorInverse(&hitPosGlobal, &hitPosLocal);
			hitPos->x = hitPosLocal.x;
			hitPos->y = hitPosLocal.y;
			hitPos->z = hitPosLocal.z;
		}
	}

	if (victimData != nullptr)
	{
		this->fireCommand(this->localTankData->turret, distance, hitPos, victimData->user->getId(), victimData->incarnation, &victimData->tank->state.pos);
	}
	else
	{
		this->fireCommand(this->localTankData->turret, distance, hitPos, "", -1, nullptr);
		if (hitPos != nullptr)
			this->battlefieldModel->addDecal(hitPos, this->barrelOrigin, 250, decalMaterial.get(), RotationState::NONE);
	}

	return 0.;
}


#ifndef L_PRO
void SmokyModel::fireCommand(ClientObjectWeapon *turret,
							 double distance,
							 const Vector3 *hitPos,
							 const QString &victimId,
							 int victimInc,
							 const Vector3 *tankPos)
{
	_function_name("SmokyModel::fireCommand");

	(void)turret;
	(void)distance;

	QString str;
	str.append("battle;fire;{\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"victimInc\":" + QString::number(victimInc));

	if (hitPos == nullptr) {
		str.append(",\"hitPos\":null,");
	} else {
		str.append(",\"hitPos\":{\"y\":");
		str.append(double_to_string(hitPos->y));
		str.append(",\"z\":");
		str.append(double_to_string(hitPos->z));
		str.append(",\"x\":");
		str.append(double_to_string(hitPos->x));
		str.append("},");
	}

	if (victimId.isEmpty()) {
		str.append("\"victimId\":null,\"tankPos\":null,\"distance\":2147483647}");
	} else {
		str.append("\"victimId\":\"");
		str.append(victimId);
		str.append("\",\"tankPos\":{\"y\":");
		str.append(double_to_string(tankPos->y));
		str.append(",\"z\":");
		str.append(double_to_string(tankPos->z));
		str.append(",\"x\":");
		str.append(double_to_string(tankPos->x));

#ifdef L_OT
		str.append("},\"distance\":" + double_to_string(distance) + "}");
#else
		str.append("},\"distance\":2147483647}");
#endif
	}

	Service::getNetworker()->send(str);
}
#else
void SmokyModel::fireCommand(ClientObjectWeapon *turret,
							 double distance,
							 const Vector3 *hitPos,
							 const QString &victimId,
							 int victimInc,
							 const Vector3 *tankPos)
{
	_function_name("SmokyModel::fireCommand");

	//battle;fire_static;{"physTime":84681,"hitPosition":{"y":583.9271429126491,"z":199.4858526720616,"x":2872.5975611528634}}

	(void)turret;
	(void)distance;
	(void)victimInc;

	QString str;

	if (victimId.isEmpty())
	{
		str.append("battle;fire_static;{\"physTime\":84681,\"hitPosition\":{\"y\":");
		str.append(double_to_string(hitPos->y));
		str.append(",\"z\":");
		str.append(double_to_string(hitPos->z));
		str.append(",\"x\":");
		str.append(double_to_string(hitPos->x));
		str.append("}}");
	}
	else
	{
		str.append("battle;fire_target;{\"physTime\":84681,\"target\":\"");
		str.append(victimId);
		str.append("\",\"incration\":");
		str.append(QString::number(0));
		str.append("\",\"targetPosition\":{\"y\":");

		str.append(double_to_string(tankPos->y));
		str.append(",\"z\":");
		str.append(double_to_string(tankPos->z));
		str.append(",\"x\":");
		str.append(double_to_string(tankPos->x));
		str.append("},\"hitPosition\":{\"y\":");
		str.append(double_to_string(hitPos->y));
		str.append(",\"z\":");
		str.append(double_to_string(hitPos->z));
		str.append(",\"x\":");
		str.append(double_to_string(hitPos->x));
		str.append("}}");
	}

	Service::getNetworker()->send(str);
}
#endif


