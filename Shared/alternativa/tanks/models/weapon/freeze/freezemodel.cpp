#include "freezemodel.h"
#include "freezedata.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "service.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "battlecontroller.h"
#include <QHash>
#include "tanks/weaponsmanager.h"
#include "../common/IWeaponCommonModel.h"
#include "alternativa/tanks/models/weapon/shared/conicareatargetsystem.h"
#include "../common/weaponcommondata.h"
#include "../weaponutils.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "../flamethrower/flamethrowertargetvalidator.h"
#include "alternativa/object/clientobjectweaponfrezee.h"
#include "sfx/freezesfxmodel.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "gametime.h"
#include "networking/network.h"
#include "double_to_string.h"
#include "alternativa/object/clientobjectuser.h"



class FreezeItemHitUser
{
public:
	QString id;
	Vector3 targetPosition;
	double targetDistance;
};







FreezeModel::FreezeModel()
{
	this->sfxModel = nullptr;
	this->currentEnergy = 0;
	this->nextTargetCheckTime = 0;
	this->lastUpdateTime = 0;
	this->active = false;
	this->targetSystem = nullptr;
	this->sfxModel = nullptr;
	this->battlefieldModel = nullptr;
	this->localTankData = nullptr;
	this->localFreezeData = nullptr;
	this->localWeaponCommonData = nullptr;
	this->tankModel = nullptr;
	this->targets = new QVector<FreezeItemHitUser*>;
}

FreezeModel::~FreezeModel()
{
	/*
	QHashIterator<QString, EffectsPair*> i(this->activeEffects);
	while (i.hasNext()) {
		i.next();
		delete i.value();
	}

	targetValidator->destroy();
	*/

	qDeleteAll(*targets);
	delete targets;
}

void FreezeModel::startFire(ClientObject *clientObject, const QString &shooterId)
{
	(void)clientObject;

	TankData *tankData = this->getTankDataSafe(shooterId);
	if (((!(tankData == nullptr)) && (!(tankData == this->tankModel->localUserData))))
	{
		this->createEffects(tankData, this->weaponCommonModel->getCommonData(tankData->turret));
	}
}

void FreezeModel::stopFire(ClientObject *clientObject, const QString &shooterId)
{
	(void)clientObject;
	TankData *tankData = this->getTankDataSafe(shooterId);
	if ((((!(tankData == nullptr)) && (tankData->enabled)) && (!(tankData == this->tankModel->localUserData))))
	{
		this->stopEffects(tankData);
	}
}

void FreezeModel::initObject(ClientObjectWeapon *clientObject,
							 double damageAreaConeAngle,
							 double damageAreaRange,
							 int energyCapacity,
							 int energyDischargeSpeed,
							 int energyRechargeSpeed,
							 int weaponTickMsec)
{
	this->cacheInterfaces();

	std::shared_ptr<FreezeData> freezeData = std::make_shared<FreezeData>(damageAreaConeAngle,
																		  100 * damageAreaRange,
																		  energyCapacity,
																		  energyDischargeSpeed,
																		  energyRechargeSpeed,
																		  weaponTickMsec);

	ClientObjectWeaponFrezee::setData(clientObject, freezeData);

	WeaponsManager::getFrezeeSFXModel(clientObject);
	if (this->sfxModel == nullptr)
	{
		this->sfxModel = WeaponsManager::getFrezeeSFXModel(clientObject);
	}
}

void FreezeModel::stopEffects(TankData *ownerTankData)
{
	if (this->sfxModel != nullptr)
	{
		this->sfxModel->destroyEffects(ownerTankData);
	}
}

void FreezeModel::reset()
{
	this->currentEnergy = this->localFreezeData->energyCapacity;
	this->lastUpdateTime = GetGameTime();
}



void FreezeModel::setLocalUser(TankData *localUserData)
{
	this->localTankData = localUserData;
	this->localFreezeData = getFreezeData(localUserData->turret);
	this->localWeaponCommonData = this->weaponCommonModel->getCommonData(localUserData->turret);
	this->currentEnergy = this->localFreezeData->energyCapacity;
	this->lastUpdateTime = 0;
	int numRays = 5;
	int numSteps = 6;
	this->targetSystem = new ConicAreaTargetSystem(this->localFreezeData->damageAreaRange,
												   this->localFreezeData->damageAreaConeAngle,
												   numRays,
												   numSteps,
												   this->battlefieldModel->getBattlefieldData()->collisionDetector,
												   new FlamethrowerTargetValidator());
}

void FreezeModel::clearLocalUser()
{
	this->localTankData = nullptr;
	this->localFreezeData = nullptr;
	this->localWeaponCommonData = nullptr;
	this->targetSystem = nullptr;
}

void FreezeModel::activateWeapon(int time)
{
	this->active = true;
	this->nextTargetCheckTime = time + this->localFreezeData->weaponTickMsec;
	this->lastUpdateTime = time;
	this->startFireCommand();
	this->createEffects(this->localTankData, this->localWeaponCommonData);
}

void FreezeModel::deactivateWeapon(int time, bool sendServerCommand)
{
	this->active = false;
	this->lastUpdateTime = time;
	if (sendServerCommand)
	{
		this->stopFireCommand();
	}
	this->stopEffects(this->localTankData);
}

double FreezeModel::update(int time, int deltaTime)
{
	(void)deltaTime;

	if (this->active)
	{
		if (time >= this->nextTargetCheckTime)
		{
			this->nextTargetCheckTime = (this->nextTargetCheckTime + this->localFreezeData->weaponTickMsec);
			this->checkTargets(this->localWeaponCommonData, this->localTankData);
		}
		this->currentEnergy = this->currentEnergy - ((this->localFreezeData->energyDischargeSpeed * (time - this->lastUpdateTime)) * 0.001);
		if (this->currentEnergy <= 0)
		{
			this->currentEnergy = 0;
			this->deactivateWeapon(time, true);
		}
	}
	else
	{
		double energyCapacity = this->localFreezeData->energyCapacity;
		if (this->currentEnergy < energyCapacity)
		{
			this->currentEnergy = this->currentEnergy + ((this->localFreezeData->energyRechargeSpeed * (time - this->lastUpdateTime)) * 0.001);
			if (this->currentEnergy > energyCapacity)
			{
				this->currentEnergy = energyCapacity;
			}
		}
	}
	this->lastUpdateTime = time;
	return this->currentEnergy / this->localFreezeData->energyCapacity;
}

void FreezeModel::checkTargets(WeaponCommonData *commonData, TankData *tankData)
{
	const Vector3 *muzzleLocalPos = commonData->getMuzzle(0);

	Vector3 muzzlePosGlobal;
	Vector3 barrelOrigin;
	Vector3 gunRotationAxis;
	Vector3 gunGlobalDir;


	WeaponUtils::calculateGunParams(tankData->tank->skin->turretMesh,
									muzzleLocalPos,
									&muzzlePosGlobal,
									&barrelOrigin,
									&gunRotationAxis,
									&gunGlobalDir);

	double barrelLength = muzzleLocalPos->y;



	QVector<Body*> targetIds;
	QVector<double> targetDistances;

	this->targetSystem->getTargets(this->localTankData->tank,
								  barrelLength,
								  0.3,
								  &barrelOrigin,
								  &gunGlobalDir,
								  &gunRotationAxis,
								  &targetIds,
								  &targetDistances);

	int len = targetIds.length();
	if (len > 0)
	{
		if (this->targets->length() < len)
			for (int i = this->targets->length(); i < len; i++)
				this->targets->append(new FreezeItemHitUser());

		for (int i = 0; i < len; i++)
		{
			FreezeItemHitUser *item = targets->at(i);
			Tank *target = (Tank*)targetIds.at(i);
			TankData *targetData = target->tankData;
			item->id = targetData->user->getId();
			item->targetDistance = 0.01 * targetDistances.at(i);
			//this->targetIncarnations[i] = targetData.incarnation;
			Vector3 &targetPosition = target->state.pos;
			item->targetPosition.x = targetPosition.x;
			item->targetPosition.y = targetPosition.y;
			item->targetPosition.z = targetPosition.z;
		}
		//this->targetIncarnations.length = len;
		//this->targetPositions.length = len;
		this->hitCommand(targets, len);
	}
}

void FreezeModel::createEffects(TankData *tankData, WeaponCommonData *commonData)
{
	if (this->sfxModel != nullptr)
	{
		this->sfxModel->createEffects(tankData, commonData);
	}
}

TankData *FreezeModel::getTankDataSafe(const QString &tankId)
{
	ClientObjectUser *tankObject = Service::getBattleController()->tanks->value(tankId);
	if (tankObject == nullptr)
		return nullptr;

	TankData *tankData = this->tankModel->getTankData(tankObject);
	if (tankData == nullptr || tankData->tank == nullptr)
		return nullptr;

	return tankData;
}

void FreezeModel::cacheInterfaces()
{
	{
		this->battlefieldModel = Service::getIBattleField();
		this->tankModel = Service::getTankModel();
		this->weaponCommonModel = Service::getIWeaponCommonModel();
		//this->weaponWeakeningModel = IWeaponWeakeningModel(this->modelService.getModelsByInterface(IWeaponWeakeningModel)[0]);
	}
}



FreezeData* FreezeModel::getFreezeData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponFrezee::data(clientObject);
}


void FreezeModel::startFireCommand()
{
	Service::getNetworker()->send("battle;start_fire");
}

void FreezeModel::stopFireCommand()
{
	Service::getNetworker()->send("battle;stop_fire");
}


void FreezeModel::hitCommand(QVector<FreezeItemHitUser*> *targets, int len)
{
	//battle;fire;{"victims":[""],"tickPeriod":500,"targetDistances":[8.847246833819076]}

	QString str;
	str.append("battle;fire;{\"victims\":[");

	for (int i = 0; i < len; i++) {
		if (i != 0)
			str.append(QChar(','));
		FreezeItemHitUser *user = targets->at(i);
		str.append(QChar('\"') + user->id + QChar('\"'));
	}

	str.append("],\"tickPeriod\":");
	str.append(QString::number(this->localFreezeData->weaponTickMsec));
	str.append(",\"targetDistances\":[");

	for (int i = 0; i < len; i++) {
		if (i != 0)
			str.append(QChar(','));
		FreezeItemHitUser *user = targets->at(i);
		str.append(double_to_string(user->targetDistance));
	}
	str.append("]}");

	Service::getNetworker()->send(str);
}

