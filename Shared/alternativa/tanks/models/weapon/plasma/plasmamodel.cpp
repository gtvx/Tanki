#include "plasmamodel.h"
#include "PlasmaGunData.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/common/weaponcommonmodel.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "service.h"
#include "battlecontroller.h"
#include "plasmashot.h"
#include "mymath.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include <QVector>
#include "alternativa/tanks/models/weapon/shared/commontargetsystem.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/models/weapon/shared/commontargetevaluator.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/object/clientobjectweapontwins.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/models/weapon/common/HitInfo.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/models/weapon/weakening/weaponweakeningmodel.h"
#include "alternativa/tanks/models/sfx/shoot/plasma/plasmasfxmodel.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/models/battlefield/decals/RotationState.h"
#include "networking/network.h"
#include "double_to_string.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"



PlasmaModel::PlasmaModel()
{
	maxTargetingDistance = 100000;
	shotId = 0;
#ifdef GRAPHICS
	decalMaterial = new_TextureMaterial();
	decalMaterial->init();
	decalMaterial->setTexture(ResourceUtil::getResourceImageLocal("decal/plasma.png"));
#endif
}

PlasmaModel::~PlasmaModel()
{

}

void PlasmaModel::objectLoaded(ClientObject*)
{
	this->cacheInterfaces();
}

void PlasmaModel::objectUnloaded(ClientObject*)
{
	/*
	var playerShots:Dictionary;
	var shot:PlasmaShot;
	for each (playerShots in this->activeShots)
	{
		for each (shot in playerShots)
		{
			shot.kill();
		}
	}
	this->activeShots = new Dictionary();
	*/
}

void PlasmaModel::initObject(ClientObjectWeapon *clientObject, double chargeRadius, double distance, double speed)
{
	std::shared_ptr<PlasmaGunData> data = std::make_shared<PlasmaGunData>();
	data->shotSpeed = (speed * 100);
	data->shotRange = (distance * 100);
	data->shotRadius = (chargeRadius * 100);
	ClientObjectWeaponTwins::setData(clientObject, data);
}


void PlasmaModel::fire(ClientObjectUser *clientObject, const QString &firingTankId, int barrel, int shotId, const Vector3 *dirToTarget)
{
	(void)clientObject;

	this->objectLoaded(nullptr);

	ClientObjectUser *firingTankObject = Service::getBattleController()->tanks->value(firingTankId);


	if (firingTankObject == nullptr)
	{
		return;
	}

	if (this->tankModel->localUserData != nullptr)
	{
		if (firingTankId == this->tankModel->localUserData->user->getId())
		{
			return;
		}
	}

	TankData *firingTankData = this->tankModel->getTankData(firingTankObject);
	if (firingTankData->tank == nullptr)
	{
		return;
	}

	Vector3 _muzzlePosGlobal;


	WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(firingTankData->turret);

	WeaponUtils::calculateGunParamsAux(firingTankData->tank->skin->turretMesh,
									   commonData->getMuzzle(barrel),
									   &_muzzlePosGlobal,
									   &_gunDirGlobal);

	this->weaponCommonModel->createShotEffects(firingTankData->turret, firingTankData->tank, barrel, &_muzzlePosGlobal, &_gunDirGlobal);
	if (shotId > -1)
	{
		Vector3 _dirToTarget;
		_dirToTarget.x = dirToTarget->x;
		_dirToTarget.y = dirToTarget->y;
		_dirToTarget.z = dirToTarget->z;
		this->createShot(false, shotId, firingTankData, &_muzzlePosGlobal, &_dirToTarget);
	}


}



void PlasmaModel::hit(ClientObjectUser *clientObject, const QString &firingTankId, int shotId, const Vector3 *affectedPoint, const QString &affectedTankId, double weakeningCoeff)
{
	(void)clientObject;


	ClientObjectUser *firingTankObject = Service::getBattleController()->tanks->value(firingTankId);;

	if (firingTankObject == nullptr || this->tankModel == nullptr)
	{
		return;
	}


	TankData *firingTankData = this->tankModel->getTankData(firingTankObject);


	if (firingTankData->tank == nullptr)
	{
		return;
	}

	QHash<int, PlasmaShot*> *tankShots = this->activeShots[firingTankId];
	if (tankShots != nullptr)
	{
		PlasmaShot *shot = tankShots->value(shotId);
		if (shot != nullptr)
		{
			this->removeShot(shot);
			shot->kill();
		}
	}

	if ((((affectedPoint == nullptr) || (MyMath::isNaN(affectedPoint->x))) || (MyMath::isNaN(affectedPoint->y))))
	{
		return;
	}

	Vector3 _hitPos;
	_hitPos.x = affectedPoint->x;
	_hitPos.y = affectedPoint->y;
	_hitPos.z = affectedPoint->z;

	if (affectedTankId != nullptr)
	{
		ClientObjectUser *affectedTankObject = Service::getBattleController()->tanks->value(affectedTankId);;

		if (affectedTankObject == nullptr)
		{
			return;
		}

		TankData *affectedTankData = this->tankModel->getTankData(affectedTankObject);
		if (affectedTankData->tank == nullptr)
		{
			return;
		}



		this->weaponCommonModel->createExplosionEffects(firingTankData->turret,
														this->battlefieldModel->getBattlefieldData()->viewport->camera,
														true,
														&_hitPos,
														&this->_gunDirGlobal,
														affectedTankData,
														weakeningCoeff);

		WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(firingTankData->turret);
		this->battlefieldModel->tankHit(affectedTankData, &this->_gunDirGlobal, weakeningCoeff * commonData->impactCoeff);
	}
	else
	{
		this->weaponCommonModel->createExplosionEffects(firingTankData->turret,
														this->battlefieldModel->getBattlefieldData()->viewport->camera,
														false, &_hitPos, nullptr, nullptr, weakeningCoeff);
		this->battlefieldModel->addDecal(affectedPoint, &this->_barrelOrigin, 100, decalMaterial.get(), RotationState::NONE);
	}
}



void PlasmaModel::setLocalUser(TankData *localUserData)
{
	this->objectLoaded(nullptr);
	this->localTankData = localUserData;

	this->localShotData = WeaponsManager::shotData(localUserData->turret->getId());
	this->localWeaponCommonData = this->weaponCommonModel->getCommonData(localUserData->turret);
	this->targetEvaluator = CommonTargetEvaluator::create(this->localTankData, this->localShotData, this->battlefieldModel, this->weaponWeakeningModel, nullptr);

	this->targetSystem = new CommonTargetSystem(this->maxTargetingDistance,
												this->localShotData->autoAimingAngleUp,
												this->localShotData->numRaysUp,
												this->localShotData->autoAimingAngleDown,
												this->localShotData->numRaysDown,
												this->battlefieldModel->getBattlefieldData()->collisionDetector,
												this->targetEvaluator);
}

void PlasmaModel::clearLocalUser()
{
	this->localTankData = nullptr;
	this->localShotData = nullptr;
	this->localWeaponCommonData = nullptr;
	this->targetSystem = nullptr;
	this->targetEvaluator = nullptr;
}



double PlasmaModel::update(int time, int deltaTime)
{
	(void)deltaTime;

	this->currentTime = time;
	if (((!(this->_triggerPressed)) || (this->currentTime < this->nextReadyTime)))
	{
		if (this->currentTime < this->nextReadyTime)
		{
			return (1 + ((this->currentTime - this->nextReadyTime) / this->localShotData->reloadMsec));
		}
		return 1;
	}

	Vector3 _muzzlePosGlobal;
	Vector3 _gunDirGlobal;
	Vector3 _xAxis;
	RayIntersection intersection;

	this->nextReadyTime = this->currentTime + this->localShotData->reloadMsec;
	BattlefieldData *bfData = this->battlefieldModel->getBattlefieldData();

	TanksCollisionDetector *collisionDetector = (TanksCollisionDetector*)bfData->physicsScene->collisionDetector;

	const Vector3 *muzzlePosLocal = this->localWeaponCommonData->getMuzzle(this->localWeaponCommonData->currBarrel);

	WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh,
									muzzlePosLocal, &_muzzlePosGlobal, &this->_barrelOrigin, &_xAxis, &_gunDirGlobal);

	bool canFire = (!(collisionDetector->intersectRay(&this->_barrelOrigin, &_gunDirGlobal, CollisionGroup::STATIC, muzzlePosLocal->y, nullptr, &intersection)));
	this->weaponCommonModel->createShotEffects(this->localTankData->turret, this->localTankData->tank, this->localWeaponCommonData->currBarrel, &_muzzlePosGlobal, &_gunDirGlobal);

	Vector3 _dirToTarget;

	int realShotId = -1;
	if (canFire)
	{
		HitInfo hitInfo;
		if (this->targetSystem->getTarget(&_muzzlePosGlobal, &_gunDirGlobal, &_xAxis, this->localTankData->tank, &hitInfo))
		{
			_dirToTarget.copy(&hitInfo.direction);
		}
		else
		{
			_dirToTarget.copy(&_gunDirGlobal);
		}

		realShotId = this->shotId;
		this->createShot(true, this->shotId, this->localTankData, &_muzzlePosGlobal, &_dirToTarget);
		this->shotId++;
	}
	else
	{
		_dirToTarget.reset();
	}
	this->fireCommand(this->localWeaponCommonData->currBarrel, realShotId, &_dirToTarget);
	this->localWeaponCommonData->currBarrel = (this->localWeaponCommonData->currBarrel + 1) % this->localWeaponCommonData->muzzleLength();
	return 0;
}

void PlasmaModel::fireCommand(int currBarrel, int realShotId, const Vector3 *_dirToTarget3d)
{
	//battle;start_fire;{"currBarrel":0,"realShotId":0,"reloadTime":360,"dirToTarget":{"y":-0.1669130160262698,"z":-0.42709453293396,"x":0.8886677135009103}}
	QString str;
	str.append("battle;start_fire;{\"currBarrel\":");
	str.append(QString::number(currBarrel));
	str.append(",\"realShotId\":");
	str.append(QString::number(realShotId));
	str.append(",\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"dirToTarget\":{\"y\":");
	str.append(double_to_string(_dirToTarget3d->y));
	str.append(",\"z\":");
	str.append(double_to_string(_dirToTarget3d->z));
	str.append(",\"x\":");
	str.append(double_to_string(_dirToTarget3d->x));
	str.append("}}");
	Service::getNetworker()->send(str);

#if 0
	var js:Object = new Object();
	js.currBarrel = currBarrel;
	js.realShotId = realShotId;
	js.dirToTarget = _dirToTarget3d;
	js.reloadTime = this->localShotData.reloadMsec.value;
	Network(Main.osgi.getService(INetworker)).send(("battle;start_fire;" + JSON.stringify(js)));
#endif
}

void PlasmaModel::activateWeapon(int time)
{
	(void)time;
	this->_triggerPressed = true;
}

void PlasmaModel::deactivateWeapon(int time, bool sendServerCommand)
{
	(void)time;
	(void)sendServerCommand;
	this->_triggerPressed = false;
}

void PlasmaModel::reset()
{
	this->_triggerPressed = false;
	this->nextReadyTime = 0;
}

void PlasmaModel::stopEffects(TankData *tankData)
{
	(void)tankData;
}

void PlasmaModel::plasmaShotDissolved(PlasmaShot *shot)
{
	this->removeShot(shot);
}


void PlasmaModel::plasmaShotHit(PlasmaShot *shot, const Vector3 *hitPoint, const Vector3 *hitDir, Body *body)
{
	Vector3 _hitPos3d;
	bool p = false;

	this->removeShot(shot);
	TankData *affectedTankData = nullptr;
	if (body != nullptr)
	{
		QHashIterator<TankData*, bool> i(*battlefieldModel->getBattlefieldData()->activeTanks);
		while (i.hasNext())
		{
			i.next();
			TankData *td = i.key();
			if (body == td->tank)
			{
				Vector3 _hitPosGlobal;
				_hitPosGlobal.diff(hitPoint, &body->state.pos);
				Vector3 _hitPosLocal;
				body->baseMatrix.transformVectorInverse(&_hitPosGlobal, &_hitPosLocal);
				_hitPos3d.x = _hitPosLocal.x;
				_hitPos3d.y = _hitPosLocal.y;
				_hitPos3d.z = _hitPosLocal.z;
				affectedTankData = td;
				p = true;
				break;
			}
		}

	}
	else
	{
		_hitPos3d.x = hitPoint->x;
		_hitPos3d.y = hitPoint->y;
		_hitPos3d.z = hitPoint->z;
		p = true;
	}


	double distance = 0.01 * shot->totalDistance;


	double weakeingCoeff = WeaponWeakeningModel::getImpactCoeff(shot->shooterData->turret, distance);
	this->weaponCommonModel->createExplosionEffects(shot->shooterData->turret, this->battlefieldModel->getBattlefieldData()->viewport->camera,
													false, hitPoint, hitDir, affectedTankData, weakeingCoeff);
	if (affectedTankData != nullptr)
	{
		const Vector3 *v = &affectedTankData->tank->state.pos;
		//this->_tankPos3d.x = v.x;
		//this->_tankPos3d.y = v.y;
		//this->_tankPos3d.z = v.z;
		this->hitCommand(shot->shotId, &_hitPos3d, affectedTankData->user->getId(), affectedTankData->incarnation, v, distance);
	}
	else
	{
		this->hitCommand(shot->shotId, &_hitPos3d, distance);
		if (p == true)
			this->battlefieldModel->addDecal(&_hitPos3d, &this->_barrelOrigin, 100, decalMaterial.get(), RotationState::NONE);
	}
}

void PlasmaModel::hitCommand(int shotId, const Vector3 *hitPos, int distance)
{
	//battle;fire;{"incr":-1,"hitPos":{"y":234.85,"z":189.07525634765543,"x":3562.3604240528953},"victimId":null,"reloadTime":360,"shotId":19,"distance":1,"tankPos":null}

	QString str;
	str.append("battle;fire;{\"incr\":-1,\"hitPos\":{\"y\":");
	str.append(double_to_string(hitPos->y));
	str.append(",\"z\":");
	str.append(double_to_string(hitPos->z));
	str.append(",\"x\":");
	str.append(double_to_string(hitPos->x));
	str.append("},\"victimId\":null,\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"shotId\":");
	str.append(QString::number(shotId));
	str.append(",\"distance\":");
	str.append(QString::number(distance));
	str.append(",\"tankPos\":null}");

	Service::getNetworker()->send(str);
}

void PlasmaModel::hitCommand(int shotId, const Vector3 *hitPos, const QString &affectedTankId, int incr, const Vector3 *tankPos, int distance)
{
	//battle;fire;{"incr":0,"hitPos":{"y":-256.51689918568655,"z":62.48386001586921,"x":-119.06469409619888},
	//"victimId":"Reemi","reloadTime":360,"shotId":4,"distance":7,"tankPos":{"y":1264.6361889596049,"z":323.6462445772799,"x":-1787.2115739899825}}

	QString str;
	str.append("battle;fire;{\"incr\":");
	str.append(QString::number(incr));
	str.append(",\"hitPos\":{\"y\":");
	str.append(double_to_string(hitPos->y));
	str.append(",\"z\":");
	str.append(double_to_string(hitPos->z));
	str.append(",\"x\":");
	str.append(double_to_string(hitPos->x));
	str.append("},\"victimId\":\"");
	str.append(affectedTankId);
	str.append("\",\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"shotId\":");
	str.append(QString::number(shotId));
	str.append(",\"distance\":");
	str.append(QString::number(distance));
	str.append(",\"tankPos\":{\"y\":");
	str.append(double_to_string(tankPos->y));
	str.append(",\"z\":");
	str.append(double_to_string(tankPos->z));
	str.append(",\"x\":");
	str.append(double_to_string(tankPos->x));
	str.append("}}");

	Service::getNetworker()->send(str);

#if 0
	var js:Object = new Object();
	js.shotId = shotId;
	js.hitPos = hitPos;
	js.victimId = affectedTankId;
	js.incr = incr;
	js.tankPos = tankPos;
	js.distance = distance;
	js.reloadTime = this->localShotData.reloadMsec.value;
	Network(Main.osgi.getService(INetworker)).send(("battle;fire;" + JSON.stringify(js)));
#endif
}

PlasmaGunData* PlasmaModel::getWeaponData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponTwins::data(clientObject);
}

void PlasmaModel::removeShot(PlasmaShot *shot)
{
	QHash<int, PlasmaShot*> *tankShots = this->activeShots[shot->shooterData->user->getId()];
	if (tankShots != nullptr)
	{
		//delete tankShots->value(shot->shotId);
		tankShots->remove(shot->shotId);
	}
}


void PlasmaModel::createShot(bool active, int shotId, TankData *tankData, const Vector3 *muzzleGlobalPos, const Vector3 *dirToTarget)
{
	PlasmaGunData *data = this->getWeaponData(tankData->turret);

	QString user_id = tankData->user->getId();

	QHash<int, PlasmaShot*> *tankShots = this->activeShots.value(user_id);

	if (tankShots == nullptr)
		this->activeShots.insert(user_id, tankShots = new QHash<int, PlasmaShot*>);

	BattlefieldData *bfData = this->battlefieldModel->getBattlefieldData();
#ifdef GRAPHICS
	PlasmaSFXModel *plasmaShootSfx = WeaponsManager::getTwinsSFX(WeaponsManager::getObjectFor(tankData->turret->getId()));
	PlasmaSFXData *plasmaData = plasmaShootSfx->getPlasmaSFXData(tankData->turret);
#else
	PlasmaSFXData *plasmaData = nullptr;
#endif
	PlasmaShot *shot = this->battlefieldModel->getObjectPoolService()->getPlasmaShot();

	shot->init(shotId, active, data, muzzleGlobalPos,
			   dirToTarget, tankData, this, plasmaData,
			   bfData->physicsScene->collisionDetector, nullptr);

	tankShots->insert(shotId, shot);
	this->battlefieldModel->addGraphicEffect(shot);
/*
	(void)active;
	(void)shotId;
	(void)tankData;
	(void)muzzleGlobalPos;
	(void)dirToTarget;
*/
}

void PlasmaModel::cacheInterfaces()
{
	this->battlefieldModel = Service::getIBattleField();
	this->tankModel = Service::getTankModel();
	this->weaponCommonModel = Service::getWeaponCommonModel();

#if 0
	if (this->modelService == null)
	{
		this->modelService = IModelService(Main.osgi.getService(IModelService));
		this->battlefieldModel = IBattleField(this->modelService.getModelsByInterface(IBattleField)[0]);
		this->tankModel = (Main.osgi.getService(ITank) as TankModel);
		this->weaponCommonModel = (Main.osgi.getService(IWeaponCommonModel) as WeaponCommonModel);
		this->weaponWeakeningModel = IWeaponWeakeningModel(this->modelService.getModelsByInterface(IWeaponWeakeningModel)[0]);
	}
#endif
}
