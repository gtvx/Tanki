#include "healinggunmodel.h"
#include <memory>
#include "mymath.h"
#include "HealingGunData.h"
#include "alternativa/object/clientobjectweaponisida.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "com/alternativaplatform/projects/tanks/client/warfare/models/tankparts/weapon/healing/IsisActionType.h"
#include "com/alternativaplatform/projects/tanks/client/warfare/models/tankparts/weapon/healing/struct/IsisAction.h"
#include "battlecontroller.h"
#include "service.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/sfx/healing/healinggunsfxmodel.h"
#include "alternativa/tanks/models/weapon/common/IWeaponCommonModel.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/weapon/healing/healingguntargetsystem.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "networking/network.h"
#include "double_to_string.h"
#include "alternativa/tanks/models/tank/TankEvent.h"
#include "alternativa/tanks/models/tank/tankeventdispatcher.h"
#include "__global.h"


static HealingGunData* getHealingData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponIsida::data(clientObject);
}

#ifdef GRAPHICS
static bool isEnemyTarget(TankData *ownerData, TankData *targetData)
{
	return ((ownerData->teamType == BattleTeamType::NONE) || (!(ownerData->teamType == targetData->teamType)));
}


static IsisActionType getEffectsMode(TankData *ownerData, TankData *targetData)
{
	bool isEnemy = isEnemyTarget(ownerData, targetData);
	return ((isEnemy) ? IsisActionType::DAMAGE : IsisActionType::HEAL);
}
#endif

HealingGunModel::HealingGunModel()
{
	this->targetSystem = new HealingGunTargetSystem();
	this->currentCharge = 0;
}

HealingGunModel::~HealingGunModel()
{

}




void HealingGunModel::initObject(ClientObjectWeapon *clientObject,
								 double angle,
								 int capacity,
								 int chargeRate,
								 int checkPeriodMsec,
								 double coneAngle,
								 int dischargeRate,
								 double radius)
{
	{
		//this->modelService = (Main.osgi.getService(IModelService) as IModelService);
		//this->bfInterface = (this->modelService.getModelsByInterface(IBattleField)[0] as IBattleField);
		//this->tankInterface = (Main.osgi.getService(ITank) as TankModel);
		//this->weaponCommonInterface = (this->modelService.getModelsByInterface(IWeaponCommonModel)[0] as IWeaponCommonModel);
		//this->sfxModel = (Main.osgi.getService(IHealingSFXModelBase) as HealingGunSFXModel);

	}

	this->bfInterface = Service::getIBattleField();
	this->tankInterface = Service::getTankModel();
	this->weaponCommonInterface = Service::getIWeaponCommonModel();
	this->sfxModel = WeaponsManager::getIsidaSFX(clientObject);


	ITankEventDispatcher *tankDispatcher = bfInterface->getTankEventDispatcher();
	tankDispatcher->addTankEventListener(TankEvent::KILLED, this);
	tankDispatcher->addTankEventListener(TankEvent::UNLOADED, this);

	std::shared_ptr<HealingGunData> data = std::make_shared<HealingGunData>();
	data->capacity = capacity;
	data->tickPeriod = checkPeriodMsec;
	data->lockAngle = coneAngle;
	data->lockAngleCos = MyMath::cos(coneAngle);
	data->maxRadius = radius * 100;
	data->maxAngle = angle;
	data->maxAngleCos = MyMath::cos(angle);
	data->dischargeRate = dischargeRate;
	data->chargeRate = chargeRate;
	ClientObjectWeaponIsida::setData(clientObject, data);
}

void HealingGunModel::init(ClientObjectWeapon *clientObject, QVector<IsisAction*> &isisActions)
{
	(void)clientObject;
#ifdef GRAPHICS
	for (IsisAction *isisAction : isisActions)
	{
		TankData *shooterData = this->getTankData(isisAction->shooterId);
		if (shooterData != nullptr)
		{
			if (isisAction->type == IsisActionType::IDLE)
			{
				this->sfxModel->createOrUpdateEffects(shooterData, isisAction->type, nullptr);
			}
			else
			{
				TankData *targetData = this->getTankData(isisAction->targetId);
				if (targetData != nullptr)
				{
					this->sfxModel->createOrUpdateEffects(shooterData, isisAction->type, targetData);
				}
			}
		}
	}
#else
	(void)isisActions;
#endif
}


void HealingGunModel::startWeapon(ClientObjectUser *clientObject, const IsisAction *isisAction)
{
	(void)clientObject;

#ifdef GRAPHICS
	TankData *shooterData = this->getTankData(isisAction->shooterId);
	if (shooterData == nullptr || !shooterData->enabled)
	{
		return;
	}
	if (this->tankInterface->localUserData != nullptr)
	{
		if (shooterData == this->tankInterface->localUserData)
		{
			return;
		}
	}
	if (isisAction->type == IsisActionType::IDLE)
	{
		this->sfxModel->createOrUpdateEffects(shooterData, isisAction->type, nullptr);
	}
	else
	{
		TankData *targetData = this->getTankData(isisAction->targetId);
		if (targetData == nullptr || !targetData->enabled)
		{
			return;
		}
		this->sfxModel->createOrUpdateEffects(shooterData, isisAction->type, targetData);
	}
#else
	(void)isisAction;
#endif
}

void HealingGunModel::stopWeapon(ClientObjectUser *clientObject, const QString &healerId)
{
	(void)clientObject;

	TankData *ownerTankData = this->getTankData(healerId);
	if (this->tankInterface->localUserData != nullptr)
	{
		if (ownerTankData == this->tankInterface->localUserData)
		{
			return;
		}
	}
#ifdef GRAPHICS
	if (ownerTankData != nullptr)
	{
		this->sfxModel->destroyEffectsByOwner(ownerTankData);
	}
#endif
}

void HealingGunModel::stopEffects(TankData *ownerTankData)
{
#ifdef GRAPHICS
	this->sfxModel->destroyEffectsByOwner(ownerTankData);
#else
	(void)ownerTankData;
#endif
}

void HealingGunModel::reset()
{
	this->currTarget = nullptr;
	this->nextTickTime = INT_MAX;
	this->currentCharge = ((this->localGunData == nullptr) ? 0 : this->localGunData->capacity);
}

void HealingGunModel::setLocalUser(TankData *localUserData)
{
	this->localTankData = localUserData;
	this->localGunData = getHealingData(localUserData->turret);
	this->localCommonData = this->weaponCommonInterface->getCommonData(localUserData->turret);
	this->currentCharge = this->localGunData->capacity;
	this->active = false;
	this->currTarget = nullptr;
}

void HealingGunModel::clearLocalUser()
{
	this->localTankData = nullptr;
	this->localGunData = nullptr;
	this->localCommonData = nullptr;
	this->active = false;
	this->currTarget = nullptr;
}

void HealingGunModel::activateWeapon(int time)
{
	if (this->active)
	{
		return;
	}
	this->active = true;
	this->lastUpdateTime = time;
	this->nextTickTime = (time + this->localGunData->tickPeriod);
	this->currTarget = this->lockTarget(this->localCommonData, this->localTankData);
	if (this->currTarget == nullptr)
	{
#ifdef GRAPHICS
		this->sfxModel->createOrUpdateEffects(this->localTankData, IsisActionType::IDLE, nullptr);
#endif
		this->startWeaponCommand();
	}
	else
	{
		this->startActiveMode(this->localTankData, this->currTarget);
	}
}

void HealingGunModel::deactivateWeapon(int time, bool sendServerCommand)
{
	if (!this->active)
		return;

	this->active = false;
	this->currTarget = nullptr;
	this->lastUpdateTime = time;
#ifdef GRAPHICS
	this->sfxModel->destroyEffectsByOwner(this->localTankData);
#endif
	if (sendServerCommand)
	{
		this->stopWeaponCommand();
	}
}

void HealingGunModel::stopWeaponCommand()
{
	Service::getNetworker()->send("battle;stop_fire");
}

double HealingGunModel::update(int time, int deltaTime)
{
	(void)deltaTime;

	if (this->active == true)
	{
		if (this->currentCharge == 0)
		{
			this->deactivateWeapon(time, true);
		}
		else
		{
			if (this->currTarget == nullptr)
			{
				this->currTarget = this->lockTarget(this->localCommonData, this->localTankData);
				if (this->currTarget != nullptr)
				{
					this->startActiveMode(this->localTankData, this->currTarget);
				}
			}
			else
			{
				if (!this->validateTarget())
				{
					this->currTarget = nullptr;
#ifdef GRAPHICS
					this->sfxModel->createOrUpdateEffects(this->localTankData, IsisActionType::IDLE, nullptr);
#endif
					this->startWeaponCommand();
				}
			}
			if (time >= this->nextTickTime)
			{
				this->nextTickTime = this->nextTickTime + this->localGunData->tickPeriod;
				if (this->currTarget != nullptr)
				{

					Vector3 *targetPos = &this->currTarget->tank->state.pos;
					Vector3 pos3d;
					pos3d.copy(targetPos);
					Vector3 vec;
					vec.diff(targetPos, &this->localTankData->tank->state.pos);
					this->actCommand(this->currTarget->incarnation, this->currTarget->user->getId(), &pos3d, vec.length());

				}
				else
				{
					this->actCommand();
				}
			}
			this->currentCharge = this->currentCharge - ((0.001 * (time - this->lastUpdateTime)) * this->localGunData->dischargeRate);
			if (this->currentCharge < 0)
			{
				this->currentCharge = 0;
			}
			this->lastUpdateTime = time;
		}
	}
	else
	{
		if (this->currentCharge < this->localGunData->capacity)
		{
			this->currentCharge = this->currentCharge + ((0.001 * (time - this->lastUpdateTime)) * this->localGunData->chargeRate);
			if (this->currentCharge > this->localGunData->capacity)
			{
				this->currentCharge = this->localGunData->capacity;
			}
			this->lastUpdateTime = time;
		}
	}
	return this->currentCharge / this->localGunData->capacity;
}

void HealingGunModel::actCommand(int victimInc, const QString &victimId, Vector3 *pos3d, int vLength)
{
	(void)victimInc;
	(void)pos3d;
	(void)vLength;

	QString str;
	str.append("battle;fire;{\"victimId\":\"");
	str.append(victimId);
	str.append("\",\"distance\":");
	str.append(QString::number(this->localGunData->maxRadius));
	str.append(",\"tickPeriod\":");
	str.append(QString::number(this->localGunData->tickPeriod));
	str.append("}");
	Service::getNetworker()->send(str);
}

void HealingGunModel::actCommand()
{
	QString str;
	str.append("battle;fire;{\"victimId\":null,\"distance\":");
	str.append(QString::number(this->localGunData->maxRadius));
	str.append(",\"tickPeriod\":");
	str.append(QString::number(this->localGunData->tickPeriod));
	str.append("}");
	Service::getNetworker()->send(str);
}

void HealingGunModel::handleTankEvent(int eventType, TankData *tankData)
{
	if (eventType == TankEvent::KILLED || eventType == TankEvent::UNLOADED)
	{
#ifdef GRAPHICS
		this->sfxModel->destroyEffectsByOwner(tankData);
#endif
		if (tankData == this->currTarget)
		{
			this->currTarget = nullptr;
#ifdef GRAPHICS
			this->sfxModel->createOrUpdateEffects(this->localTankData, IsisActionType::IDLE, nullptr);
#endif
			this->startWeaponCommand();
		}
		else
		{
#ifdef GRAPHICS
			this->sfxModel->destroyEffectsByTarget(tankData);
#endif
		}
	}

}


TankData* HealingGunModel::lockTarget(WeaponCommonData *commonData, TankData *weaponOwnerData)
{
	Vector3 muzzlePosGlobal;
	Vector3 barrelOrigin;
	Vector3 axisX;
	Vector3 axisY;

	const Vector3 *v = commonData->getMuzzle(0);
	WeaponUtils::calculateGunParams(weaponOwnerData->tank->skin->turretMesh,
									v, &muzzlePosGlobal, &barrelOrigin, &axisX, &axisY);

	BattlefieldData *bfData = this->bfInterface->getBattlefieldData();
	QHash<Body*, TankData*> *tankDatas = (QHash<Body*, TankData*>*)bfData->tanks;

	return this->targetSystem->getTarget(weaponOwnerData,
										this->localGunData,
										v->y,
										&barrelOrigin,
										&axisY,
										&axisX,
										5,
										6,
										(TanksCollisionDetector*)bfData->physicsScene->collisionDetector,
										*tankDatas);
}


bool HealingGunModel::validateTarget()
{
	Vector3 muzzlePosGlobal;
	Vector3 barrelOrigin;
	Vector3 axisX;
	Vector3 axisY;

	const Vector3 *v = this->localCommonData->getMuzzle(0);
	WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh,
									v, &muzzlePosGlobal, &barrelOrigin, &axisX, &axisY);

	BattlefieldData *bfData = this->bfInterface->getBattlefieldData();
	return this->targetSystem->validateTarget(this->localTankData,
											  this->currTarget,
											  this->localGunData,
											  &axisY,
											  &barrelOrigin,
											  v->y,
											  bfData->collisionDetector);
}

void HealingGunModel::startActiveMode(TankData *shooterData, TankData *targetData)
{
#ifdef GRAPHICS
	IsisActionType mode = getEffectsMode(shooterData, targetData);
	this->sfxModel->createOrUpdateEffects(shooterData, mode, targetData);
#else
	(void)shooterData;
#endif
	this->startWeaponCommand(targetData->incarnation, targetData->user->getId());
}


void HealingGunModel::startWeaponCommand()
{
	Service::getNetworker()->send("battle;start_fire;{\"victimId\":null,\"incId\":0}");
}

void HealingGunModel::startWeaponCommand(int incId, const QString &victimId)
{
	QString str;
	str.append("battle;start_fire;{\"victimId\":\"");
	str.append(victimId);
	str.append("\",\"incId\":");
	str.append(QString::number(incId));
	str.append("}");
	Service::getNetworker()->send(str);
}


TankData* HealingGunModel::getTankData(const QString &userId)
{
	BattleController *battleController = Service::getBattleController();
	ClientObjectUser *userObject = battleController->tanks->value(userId);
	if (userObject == nullptr)
		return nullptr;

	TankData *tankData = this->tankInterface->getTankData(userObject);
	if (tankData == nullptr || tankData->tank == nullptr)
		return nullptr;

	return tankData;
}
