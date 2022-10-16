#include "flamethrowermodel.h"
#include "alternativa/object/clientobjectweapon.h"
#include "alternativa/object/clientobjectweaponflamethrower.h"
#include "alternativa/object/clientobjectuser.h"
#include "FlamethrowerData.h"
#include "service.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/common/weaponcommonmodel.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/models/weapon/shared/conicareatargetsystem.h"
#include "alternativa/tanks/models/weapon/shared/conicareatargetsystembot.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "flamethrowertargetvalidator.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/physics/bodystate.h"
#include "networking/network.h"
#include "gametime.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "double_to_string.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/models/sfx/flame/flamethrowersfxmodel.h"
#include "alternativa/tanks/sfx/effectspair.h"
#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "__global.h"
#include <QStringList>
#include <QVector>
#include <QDebug>



class FlamethrowerItemHitUser
{
public:
	QString id;
	Vector3 targetPosition;
	double targetDistance;
};


FlamethrowerModel::FlamethrowerModel()
{
	targetSystem = nullptr;
	targetSystemBot = nullptr;
	localTankData = nullptr;
	battlefield = nullptr;
	tankInterface = nullptr;
	commonModel = nullptr;
	localFlameData = nullptr;
	localWeaponCommonData = nullptr;
	lastUpdateTime = 0;
	currentHeat = 0;
	targetValidator = new FlamethrowerTargetValidator;
	active = false;
	targets = new QVector<FlamethrowerItemHitUser*>;
}

FlamethrowerModel::~FlamethrowerModel()
{
	QHashIterator<QString, EffectsPair*> i(this->activeEffects);
	while (i.hasNext()) {
		i.next();
		delete i.value();
	}

	targetValidator->destroy();

	qDeleteAll(*targets);
	delete targets;
}


void FlamethrowerModel::initObject(ClientObjectWeapon *clientObject, double coneAngle, int coolingSpeed, int heatingLimit, int heatingSpeed, double range, int targetDetectionIntervalMsec)
{
	/*
	if (this->modelService == null)
	{
		this->modelService = (Main.osgi.getService(IModelService) as IModelService);
		this->battlefield = (this->modelService.getModelsByInterface(IBattleField)[0] as IBattleField);
		this->tankInterface = (this->modelService.getModelsByInterface(ITank)[0] as TankModel);
		this->commonModel = (this->modelService.getModelsByInterface(IWeaponCommonModel)[0] as IWeaponCommonModel);
		this->weakeningModel = (this->modelService.getModelsByInterface(IWeaponWeakeningModel)[0] as IWeaponWeakeningModel);
	}
	*/

	this->weakeningModel = nullptr;


	this->battlefield = Service::getBattlefieldModel();
	this->tankInterface = Service::getTankModel();
	this->commonModel = Service::getWeaponCommonModel();


	std::shared_ptr<FlamethrowerData> data = std::make_shared<FlamethrowerData>();
	data->coneAngle = coneAngle;
	data->coolingSpeed = coolingSpeed;
	data->heatingSpeed = heatingSpeed;
	data->heatLimit = heatingLimit;
	data->range = range * 100;
	data->targetDetectionInterval = targetDetectionIntervalMsec;
	ClientObjectWeaponFlamethrower::setData(clientObject, data);
}

void FlamethrowerModel::startFire(ClientObjectUser *clientObject, const QString &firingTankId)
{

	ClientObjectUser *firingTankObject = clientObject;
	if (firingTankObject == nullptr)
		return;

	TankData *firingTankData = this->tankInterface->getTankData(firingTankObject);
	if (firingTankData == nullptr)
		return;
	if (firingTankData->tank == nullptr)
		return;
	if (firingTankData->enabled == false)
		return;

	if (this->tankInterface->localUserData != nullptr)
		if (firingTankId == this->tankInterface->localUserData->user->getId())
			return;

	this->addFlameEffects(this->commonModel->getCommonData(firingTankData->turret), firingTankData);
}

void FlamethrowerModel::stopFire(ClientObjectUser *clientObject, const QString &firingTankId)
{
	(void)clientObject;
	this->removeFlameEffects(firingTankId);
}


void FlamethrowerModel::setLocalUser(TankData *localUserData)
{
	this->localTankData = localUserData;
	this->localFlameData = ClientObjectWeaponFlamethrower::data(localUserData->turret);
	this->localWeaponCommonData = this->commonModel->getCommonData(localUserData->turret);
	this->muzzle.copy(this->localWeaponCommonData->getMuzzle(0));
	this->lastUpdateTime = 0;
	this->currentHeat = 0;
	this->targetSystem = new ConicAreaTargetSystem(this->localFlameData->range,
												   this->localFlameData->coneAngle, 5, 6,
												   this->battlefield->getBattlefieldData()->collisionDetector, this->targetValidator);

	this->targetSystemBot = new ConicAreaTargetSystemBot(this->localFlameData->range,
														 this->localFlameData->coneAngle, 5, 1,
														 this->battlefield->getBattlefieldData()->collisionDetector, this->targetValidator);
}

void FlamethrowerModel::clearLocalUser()
{
	//if (this->targets != nullptr)
		//delete this->targets;

	//if (this->targetValidator != nullptr)
		//this->targetValidator->destroy();

	if (this->targetSystem != nullptr)
		delete this->targetSystem;

	if (this->targetSystemBot != nullptr)
		delete this->targetSystemBot;

	//this->targets = nullptr;
	//this->targetValidator = nullptr;
	this->localTankData = nullptr;
	this->localFlameData = nullptr;
	this->localWeaponCommonData = nullptr;
	this->targetSystem = nullptr;
}

double FlamethrowerModel::update(int time, int deltaTime)
{
	(void)deltaTime;

	if (this->active == false)
	{
		if (this->currentHeat > 0)
		{
			this->currentHeat = this->currentHeat - ((0.001 * (time - this->lastUpdateTime)) * this->localFlameData->coolingSpeed);
			if (this->currentHeat < 0)
			{
				this->currentHeat = 0;
			}
		}
		this->lastUpdateTime = time;
		return 1. - this->currentHeat / this->localFlameData->heatLimit;
	}

	if (this->currentHeat >= this->localFlameData->heatLimit)
	{
		this->deactivateWeapon(time, true);
		return 0.;
	}

	if (time >= this->nextTargetCheckTime)
	{
		Vector3 gunGlobalDir;
		Vector3 tmpVector;
		Vector3 firingOrigin;
		Vector3 xAxis;

		WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh, &muzzle, &tmpVector, &firingOrigin, &xAxis, &gunGlobalDir);

		QVector<Body*> targetIds;
		QVector<double> targetDistances;

		this->targetSystem->getTargets(this->localTankData->tank,
									   muzzle.y,
									   0.3,
									   &firingOrigin,
									   &gunGlobalDir,
									   &xAxis,
									   &targetIds,
									   &targetDistances);

		int len = targetIds.length();

		if (len > 0)
		{
			if (targets->length() < len)
				for (int i = targets->length(); i < len; i++)
					targets->append(new FlamethrowerItemHitUser());

			for (int i = 0; i < len; i++)
			{
				FlamethrowerItemHitUser *item = targets->at(i);
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

			this->hitCommand(targets, len);
		}
		this->nextTargetCheckTime  += this->localFlameData->targetDetectionInterval;
	}

	this->currentHeat = this->currentHeat + ((0.001 * (time - this->lastUpdateTime)) * this->localFlameData->heatingSpeed);
	this->lastUpdateTime = time;
	if (this->currentHeat > this->localFlameData->heatLimit)
		return 0.;

	return 1. - (this->currentHeat / this->localFlameData->heatLimit);
}

void FlamethrowerModel::startFireCommand()
{
	Service::getNetworker()->send("battle;start_fire");
}

void FlamethrowerModel::stopFireCommand()
{
	Service::getNetworker()->send("battle;stop_fire");
}

void FlamethrowerModel::activateWeapon(int time)
{
	this->active = true;
	this->nextTargetCheckTime = time + this->localFlameData->targetDetectionInterval;
	this->lastUpdateTime = time;
	this->addFlameEffects(this->localWeaponCommonData, this->localTankData);
	this->startFireCommand();
}

void FlamethrowerModel::reset()
{
	this->currentHeat = 0;
	this->nextTargetCheckTime = 0;
	this->lastUpdateTime = GetGameTime();
}

void FlamethrowerModel::stopEffects(TankData *ownerTankData)
{
	this->removeFlameEffects(ownerTankData->user->getId());
}

void FlamethrowerModel::deactivateWeapon(int time, bool sendServerCommand)
{
	this->active = false;
	this->nextTargetCheckTime = 0;
	this->lastUpdateTime = time;
	this->removeFlameEffects(this->localTankData->user->getId());
	if (sendServerCommand)
		this->stopFireCommand();
}

void FlamethrowerModel::addFlameEffects(WeaponCommonData *commonData, TankData *tankData)
{
#ifdef GRAPHICS
	QString id = tankData->user->getId();

	if (this->activeEffects.contains(id))
		return;

	IFlamethrowerSFXModel *sfxModel = WeaponsManager::getFlamethrowerSFX(tankData->turret);
	EffectsPair *effects = new EffectsPair;
	sfxModel->getSpecialEffects(tankData,
								commonData->getMuzzle(commonData->currBarrel),
								tankData->tank->skin->turretMesh,
								this->weakeningModel,
								effects);

	this->activeEffects.insert(id, effects);
	this->battlefield->addGraphicEffect(effects->graphicEffect);
	//this->battlefield->addSound3DEffect(effects.soundEffect);
#else
	(void)commonData;
	(void)tankData;
#endif
}

void FlamethrowerModel::removeFlameEffects(const QString &userId)
{
	EffectsPair *effectsPair = this->activeEffects.value(userId);
	if (effectsPair != nullptr)
	{
		this->activeEffects.remove(userId);

		if (effectsPair->graphicEffect != nullptr)
		{
			effectsPair->graphicEffect->kill();
		}

		//if (effectsPair->soundEffect != nullptr)
			//effectsPair->soundEffect->kill();

		delete effectsPair;
	}
}



void FlamethrowerModel::hitCommand(QVector<FlamethrowerItemHitUser*> *targets, int len)
{
	QString str;
	str.append("battle;fire;{\"targetsIds\":[");

	for (int i = 0; i < len; i++) {
		if (i != 0)
			str.append(QChar(','));
		FlamethrowerItemHitUser *user = targets->at(i);
		str.append(QChar('\"') + user->id + QChar('\"'));
	}

	str.append("],\"targetPositions\":[");
	for (int i = 0; i < len; i++) {
		if (i != 0)
			str.append(QChar(','));
		FlamethrowerItemHitUser *user = targets->at(i);
		str.append("{\"y\":");
		str.append(double_to_string(user->targetPosition.y));
		str.append(",\"z\":");
		str.append(double_to_string(user->targetPosition.z));
		str.append(",\"x\":");
		str.append(double_to_string(user->targetPosition.x));
		str.append(QChar('}'));
	}
	str.append("],\"tickPeriod\":");
	str.append(QString::number(this->localFlameData->targetDetectionInterval));
	str.append(",\"targetDistances\":[");

	for (int i = 0; i < len; i++) {
		if (i != 0)
			str.append(QChar(','));
		FlamethrowerItemHitUser *user = targets->at(i);
		str.append(double_to_string(user->targetDistance));
	}
	str.append("]}");


	Service::getNetworker()->send(str);
}


FlamethrowerData* FlamethrowerModel::getFlameData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponFlamethrower::data(clientObject);
}

