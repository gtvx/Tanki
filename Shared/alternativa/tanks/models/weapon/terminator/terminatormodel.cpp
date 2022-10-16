#include "terminatormodel.h"
#include "TerminatorData.h"
#include "alternativa/object/clientobjectweaponterminator.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/sfx/shoot/railgun/railgunsfxmodel.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "../common/IWeaponCommonModel.h"
#include "service.h"
#include "terminatortargetsystem.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "TerminatorShotResult.h"
#include "mymath.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "networking/network.h"
#include "double_to_string.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/models/battlefield/decals/RotationState.h"
#include "alternativa/tanks/models/weapon/WeaponConst.h"
#include "alternativa/object/clientobjectuser.h"
#include "battlecontroller.h"
#include <QString>


static const double DECAL_RADIUS = 50;
static const double INFINITY = 20000;

static TerminatorData* getTerminatorData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponTerminator::data(clientObject);
}


class TerminatorItemHitUser
{
public:
	QString id;
	Vector3 targetPosition;
	Vector3 hitPoint;
	int targetIncarnation;
};



TerminatorModel::TerminatorModel()
{
	double_shoot = false;
	targetSystem = new TerminatorTargetSystem();
	shotResult = new TerminatorShotResult();
	tankModel = nullptr;
	battlefieldModel = nullptr;
	commonModel = nullptr;
	localTankData = nullptr;
	localShotData = nullptr;
	localTerminatorData = nullptr;
	localWeaponCommonData = nullptr;
	_triggerPressed = false;
	chargeTimeLeft = 0;
	nextReadyTime = 0;
	firstshot = true;
	shotBarrel = 0;
	activatedTime = 0;
	quickShot = false;
	lock = false;
	double_shoot = false;
	defaultReloadMsec = 1;
	targets = new QVector<TerminatorItemHitUser*>;


#ifdef GRAPHICS
	decalMaterial = new_TextureMaterial();
	decalMaterial->init();
	decalMaterial->setTexture(ResourceUtil::getResourceImageLocal("decal/railgun.png"));
#endif
}

TerminatorModel::~TerminatorModel()
{
	qDeleteAll(*targets);
	delete targets;
}

void TerminatorModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;

	if (this->commonModel == nullptr)
	{
		this->battlefieldModel = Service::getBattlefieldModel();
		this->tankModel = Service::getTankModel();
		this->commonModel = Service::getIWeaponCommonModel();
	}
}

void TerminatorModel::objectUnloaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
}

void TerminatorModel::initObject(ClientObjectWeapon *clientObject, int chargingTimeMsec, double weakeningCoeff)
{
	std::shared_ptr<TerminatorData> terminatorData = std::make_shared<TerminatorData>();
	terminatorData->chargingTime = chargingTimeMsec;
	terminatorData->weakeningCoeff = weakeningCoeff;
	ClientObjectWeaponTerminator::setData(clientObject, terminatorData);
	//this->battlefieldModel.blacklist.push(decalMaterial.getTextureResource());
	this->objectLoaded(clientObject);
}

void TerminatorModel::startFire(ClientObjectUser *clientObject, int barrel)
{
	ClientObjectUser *firingTank = clientObject;
	if (firingTank == nullptr)
	{
		return;
	}
	if (this->tankModel == nullptr)
	{
		this->tankModel = Service::getITank();
	}
	TankData *firingTankData = this->tankModel->getTankData(firingTank);
	if (firingTankData->tank == nullptr || !firingTankData->enabled || firingTankData->local)
	{
		return;
	}
	if (this->commonModel == nullptr)
	{
		this->commonModel = Service::getIWeaponCommonModel();
	}

	WeaponCommonData *commonData = this->commonModel->getCommonData(firingTankData->turret);
	this->shotBarrel = barrel;
	Vector3 _globalMuzzlePosition;
	Vector3 _globalGunDirection;
	WeaponUtils::calculateGunParamsAux(firingTankData->tank->skin->turretMesh,
									   commonData->getMuzzle(barrel),
									   &_globalMuzzlePosition,
									   &_globalGunDirection);


#ifdef GRAPHICS
	IRailgunSFXModel *terminatorSfxModel = WeaponsManager::getTerminatorSFX(firingTankData->turret);
	TerminatorData *terminatorData = getTerminatorData(firingTankData->turret);
	RailgunShootSFXData *swfData = ClientObjectWeaponTerminator::SWFData(firingTankData->turret);
	IGraphicEffect *graphicEffect = terminatorSfxModel->createChargeEffect(firingTankData->turret,
																		   firingTankData->user,
																		   swfData,
																		   commonData->getMuzzle(barrel),
																		   firingTankData->tank->skin->turretMesh,
																		   terminatorData->chargingTime);
	if (this->battlefieldModel == nullptr)
	{
		this->battlefieldModel = Service::getBattlefieldModel();
	}
	if (this->firstshot)
	{
		this->firstshot = false;
		this->battlefieldModel->addGraphicEffect(graphicEffect);
		graphicEffect = terminatorSfxModel->createChargeEffect(firingTankData->turret,
															   firingTankData->user,
															   swfData,
															   commonData->getMuzzle(barrel),
															   firingTankData->tank->skin->turretMesh,
															   terminatorData->chargingTime);
	}
	this->battlefieldModel->addGraphicEffect(graphicEffect);
#endif
	/*
	ISound3DEffect *soundEffect = terminatorSfxModel.createSoundShotEffect(firingTankData.turret, firingTankData.user, this->_globalMuzzlePosition);
	if (soundEffect != nullptr)
	{
		this->battlefieldModel->addSound3DEffect(soundEffect);
	}
	*/
}


void TerminatorModel::fire(ClientObjectUser *clientObject, QVector<Vector3*> &affectedPoints, QStringList &affectedTankIds, bool doubleShot)
{
	ClientObjectUser *firingTank = clientObject;
	if (firingTank == nullptr)
	{
		return;
	}

	TankData *firingTankData = this->tankModel->getTankData(firingTank);
	if (((((firingTankData == nullptr) || (firingTankData->tank == nullptr)) || (!(firingTankData->enabled))) || (firingTankData->local)))
	{
		return;
	}
	if (this->commonModel == nullptr)
	{
		this->commonModel = Service::getIWeaponCommonModel();
	}
	if (this->battlefieldModel == nullptr)
	{
		this->battlefieldModel = Service::getBattlefieldModel();
	}

	Vector3 _globalMuzzlePosition;
	Vector3 _globalGunDirection;

	WeaponCommonData *commonData = this->commonModel->getCommonData(firingTankData->turret);
	TerminatorData *terminatorData = getTerminatorData(firingTankData->turret);
	WeaponUtils::calculateGunParamsAux(firingTankData->tank->skin->turretMesh,
									   commonData->getMuzzle(this->shotBarrel),
									   &_globalMuzzlePosition, &_globalGunDirection);

	int numPoints = affectedPoints.length();
	double impactForce = commonData->impactForce;
	if (!affectedTankIds.isEmpty())
	{
		int i = 0;
		while (i < (numPoints - 1))
		{
			ClientObjectUser *affectedTankObject = Service::getBattleController()->tanks->value(affectedTankIds.at(i));
			if (affectedTankObject != nullptr)
			{
				TankData *affectedTankData = this->tankModel->getTankData(affectedTankObject);
				if (!((affectedTankData == nullptr) || (affectedTankData->tank == nullptr)))
				{
					const Vector3 *v = affectedPoints.at(i);
					Vector3 _globalHitPosition;
					_globalHitPosition.x = v->x;
					_globalHitPosition.y = v->y;
					_globalHitPosition.z = v->z;
					_globalHitPosition.transformBy3(&affectedTankData->tank->baseMatrix);
					_globalHitPosition.add(&affectedTankData->tank->state.pos);
					affectedTankData->tank->addWorldForceScaled(&_globalHitPosition, &_globalGunDirection, ((doubleShot) ? (impactForce / 1.5) : impactForce));
					this->battlefieldModel->tankHit(affectedTankData, &_globalGunDirection, (impactForce / WeaponConst::BASE_IMPACT_FORCE));
					impactForce = impactForce * terminatorData->weakeningCoeff;
				}
			}
			i++;
		}
	}
	const Vector3 *v = affectedPoints.at(numPoints - 1);
	Vector3 _globalHitPosition;
	_globalHitPosition.x = v->x;
	_globalHitPosition.y = v->y;
	_globalHitPosition.z = v->z;
	this->battlefieldModel->addDecal(&_globalHitPosition, &_globalMuzzlePosition, DECAL_RADIUS, decalMaterial.get(), RotationState::NONE);
	firingTankData->tank->addWorldForceScaled(&_globalMuzzlePosition, &_globalGunDirection, ((doubleShot) ? (-(commonData->kickback) / 1.5) : -(commonData->kickback)));

#ifdef GRAPHICS
	IRailgunSFXModel *terminatorSfxModel = WeaponsManager::getTerminatorSFX(firingTankData->turret);
	Vector3 v2;
	v2.copy(&_globalHitPosition);
	v2.remove(&_globalMuzzlePosition);
	v2.normalize();
	double angle = v2.cosAngle(&_globalGunDirection);


	IGraphicEffect *graphicEffect;

	if (angle < 0)
	{
		Vector3 v;
		v.copy(&_globalMuzzlePosition);
		v.scale(2);
		v.remove(&_globalHitPosition);
		graphicEffect = terminatorSfxModel->createGraphicShotEffect(firingTankData->turret, &_globalMuzzlePosition, &v);
	}
	else
	{
		graphicEffect = terminatorSfxModel->createGraphicShotEffect(firingTankData->turret, &_globalMuzzlePosition, &_globalHitPosition);
	}

	if (graphicEffect != nullptr)
	{
		this->battlefieldModel->addGraphicEffect(graphicEffect);
	}
#endif
}

void TerminatorModel::activateWeapon(int time)
{
	(void)time;
	this->_triggerPressed = true;
}

void TerminatorModel::deactivateWeapon(int time, bool sendServerCommand)
{
	(void)time;
	(void)sendServerCommand;
	this->_triggerPressed = false;
}

void TerminatorModel::setLocalUser(TankData *localUserData)
{
	this->localTankData = localUserData;
	this->localShotData = WeaponsManager::shotData(localUserData->turret->getId());
	this->defaultReloadMsec = this->localShotData->reloadMsec;
	this->localTerminatorData = getTerminatorData(localUserData->turret);
	this->localWeaponCommonData = this->commonModel->getCommonData(localUserData->turret);
	ICTFModel *ctfModel = nullptr;
	this->battlefieldModel = Service::getBattlefieldModel();
	this->targetSystem->setParams(this->battlefieldModel->getBattlefieldData()->physicsScene->collisionDetector,
								 this->localShotData->autoAimingAngleUp,
								 this->localShotData->numRaysUp,
								 this->localShotData->autoAimingAngleDown,
								 this->localShotData->numRaysDown,
								 this->localTerminatorData->weakeningCoeff,
								 ctfModel);
	this->reset();
#ifdef GRAPHICS
	const Vector3 *muzzleLocalPos = this->localWeaponCommonData->getMuzzle(this->localWeaponCommonData->currBarrel);
	IRailgunSFXModel *terminatorSfxModel = WeaponsManager::getTerminatorSFX(this->localTankData->turret);
	RailgunShootSFXData *swfData = ClientObjectWeaponTerminator::SWFData(localTankData->turret);

	IGraphicEffect *graphicEffect = terminatorSfxModel->createChargeEffect(this->localTankData->turret,
																		   this->localTankData->user,
																		   swfData,
																		   muzzleLocalPos,
																		   this->localTankData->tank->skin->turretMesh,
																		   this->localTerminatorData->chargingTime);
	if (graphicEffect != nullptr)
	{
		this->battlefieldModel->addGraphicEffect(graphicEffect);
	}
#endif
}

void TerminatorModel::clearLocalUser()
{
	this->localTankData = nullptr;
	this->localShotData = nullptr;
	this->localTerminatorData = nullptr;
	this->localWeaponCommonData = nullptr;
}




double TerminatorModel::update(int time, int deltaTime)
{
	if (this->chargeTimeLeft > 0)
	{
		this->chargeTimeLeft = this->chargeTimeLeft - deltaTime;
		if (this->chargeTimeLeft <= 0)
		{
			this->chargeTimeLeft = 0;
			if (this->double_shoot == true)
			{
				this->doFire(this->localWeaponCommonData, this->localTankData, time, true);
				this->doFire(this->localWeaponCommonData, this->localTankData, time, true);
				this->double_shoot = false;
			}
			else
			{
				this->doFire(this->localWeaponCommonData, this->localTankData, time, false);
			}
		}
		return (double)this->chargeTimeLeft / this->localTerminatorData->chargingTime;
	}
	if (time < this->nextReadyTime)
	{
		return 1 - (double(this->nextReadyTime - time) / this->localShotData->reloadMsec);
	}
	if ((((this->_triggerPressed) && (TankData::localTankData->spawnState == TankSpawnState::ACTIVE)) && (time > this->nextReadyTime)))
	{
		this->activatedTime = (this->activatedTime + deltaTime);
	}
	else
	{
		if ((((TankData::localTankData->spawnState == TankSpawnState::ACTIVE) && (this->activatedTime > 0)) && (this->activatedTime < 1000)))
		{
			this->quickShot = true;
		}
		this->activatedTime = 0;
	}

	Vector3 _globalMuzzlePosition;
	Vector3 _barrelOrigin;
	Vector3 _xAxis;
	Vector3 _globalGunDirection;


	if (this->activatedTime >= 1000)
	{
		if (!this->lock)
		{
			this->double_shoot = true;
			this->chargeTimeLeft = this->localTerminatorData->chargingTime;
			const Vector3 *muzzleLocalPos = this->localWeaponCommonData->getMuzzle(this->localWeaponCommonData->currBarrel);


			WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh,
												 muzzleLocalPos,
												 &_globalMuzzlePosition,
												 &_barrelOrigin,
												 &_xAxis,
												 &_globalGunDirection);

			RailgunSFXModel *terminatorSfxModel = WeaponsManager::getTerminatorSFX(this->localTankData->turret);
			RailgunShootSFXData *swfData = ClientObjectWeaponTerminator::SWFData(localTankData->turret);

			IGraphicEffect *graphicEffect = terminatorSfxModel->createChargeEffect(this->localTankData->turret,
																				   this->localTankData->user,
																				   swfData,
																				   muzzleLocalPos,
																				   this->localTankData->tank->skin->turretMesh,
																				   this->localTerminatorData->chargingTime);
			if (graphicEffect != nullptr)
			{
				this->battlefieldModel->addGraphicEffect(graphicEffect);
			}

			/*
			ISound3DEffect *soundEffect = terminatorSfxModel.createSoundShotEffect(this->localTankData->turret,
																				   this->localTankData->user,
																				   this->_globalMuzzlePosition);
			if (soundEffect != nullptr)
			{
				this->battlefieldModel->addSound3DEffect(soundEffect);
			}
			*/


			this->startFireCommand(this->localWeaponCommonData->currBarrel);
			this->localWeaponCommonData->currBarrel = ((this->localWeaponCommonData->currBarrel + 1) % this->localWeaponCommonData->muzzleLength());
			const Vector3 *secondMuzzleLocalPos = this->localWeaponCommonData->getMuzzle(this->localWeaponCommonData->currBarrel);
			WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh, secondMuzzleLocalPos, &_globalMuzzlePosition, &_barrelOrigin, &_xAxis, &_globalGunDirection);
			IGraphicEffect *secondGraphicEffect = terminatorSfxModel->createChargeEffect(this->localTankData->turret,
																						 this->localTankData->user,
																						 swfData,
																						 secondMuzzleLocalPos,
																						 this->localTankData->tank->skin->turretMesh,
																						 this->localTerminatorData->chargingTime);
			if (secondGraphicEffect != nullptr)
			{
				this->battlefieldModel->addGraphicEffect(secondGraphicEffect);
			}
			this->startFireCommand(this->localWeaponCommonData->currBarrel);
			this->localShotData->reloadMsec = (this->defaultReloadMsec * 1.75);
			this->activatedTime = 0;
		}
	}
	else
	{
		if (this->quickShot)
		{
			this->chargeTimeLeft = this->localTerminatorData->chargingTime;
			const Vector3 *muzzleLocalPos = this->localWeaponCommonData->getMuzzle(this->localWeaponCommonData->currBarrel);
			WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh,
												 muzzleLocalPos,
												 &_globalMuzzlePosition,
												 &_barrelOrigin,
												 &_xAxis,
												 &_globalGunDirection);

			RailgunSFXModel *terminatorSfxModel = WeaponsManager::getTerminatorSFX(this->localTankData->turret);
			RailgunShootSFXData *swfData = ClientObjectWeaponTerminator::SWFData(localTankData->turret);

			IGraphicEffect *graphicEffect = terminatorSfxModel->createChargeEffect(this->localTankData->turret,
																				   this->localTankData->user,
																				   swfData,
																				   muzzleLocalPos,
																				   this->localTankData->tank->skin->turretMesh,
																				   this->localTerminatorData->chargingTime);
			if (graphicEffect != nullptr)
			{
				this->battlefieldModel->addGraphicEffect(graphicEffect);
			}
			/*
			ISound3DEffect *soundEffect = terminatorSfxModel->createSoundShotEffect(this->localTankData->turret,
																					this->localTankDatavuser,
																					this->_globalMuzzlePosition);
			if (soundEffect != nullptr)
			{
				this->battlefieldModel->addSound3DEffect(soundEffect);
			}
			*/

			this->startFireCommand(this->localWeaponCommonData->currBarrel);
			if (this->localShotData->reloadMsec > this->defaultReloadMsec)
			{
				this->localShotData->reloadMsec = this->localShotData->reloadMsec / 1.75;
			}
			this->quickShot = false;
			this->activatedTime = 0;
		}
	}

	return 1;
}

void TerminatorModel::startFireCommand(int currBarrel)
{
	QString str;
	str.append("battle;start_fire;{\"currBarrel\":");
	str.append(QChar('0' + currBarrel));
	str.append("}");
	Service::getNetworker()->send(str);
}

void TerminatorModel::reset()
{
	this->activatedTime = 0;
	this->nextReadyTime = this->chargeTimeLeft = 0;
	this->_triggerPressed = false;
}

void TerminatorModel::stopEffects(TankData *ownerTankData)
{
	(void)ownerTankData;
}

void TerminatorModel::doFire(WeaponCommonData *commonData, TankData *tankData, int time, bool double_shoot)
{
	Vector3 _globalMuzzlePosition;
	Vector3 _barrelOrigin;
	Vector3 _xAxis;
	Vector3 _globalGunDirection;

	this->nextReadyTime = time + this->localShotData->reloadMsec;
	WeaponUtils::calculateGunParams(tankData->tank->skin->turretMesh,
									commonData->getMuzzle(commonData->currBarrel),
									&_globalMuzzlePosition,
									&_barrelOrigin,
									&_xAxis,
									&_globalGunDirection);

	BattlefieldData *bfData = this->battlefieldModel->getBattlefieldData();

	QHash<Body*, TankData*> *tanks = (QHash<Body*, TankData*>*)bfData->tanks;

	this->targetSystem->getTargets(tankData, &_barrelOrigin, &_globalGunDirection, &_xAxis, tanks, this->shotResult);


	Vector3 _globalHitPosition;
	if (this->shotResult->hitPoints.length() == 0)
	{
		Vector3 _hitPos3d;
		_globalHitPosition.x = _hitPos3d.x = (_globalMuzzlePosition.x + (INFINITY * _globalGunDirection.x));
		_globalHitPosition.y = _hitPos3d.y = (_globalMuzzlePosition.y + (INFINITY * _globalGunDirection.y));
		_globalHitPosition.z = _hitPos3d.z = (_globalMuzzlePosition.z + (INFINITY * _globalGunDirection.z));
		this->fireCommand(&_hitPos3d,  this->localWeaponCommonData->currBarrel, double_shoot);
		this->localWeaponCommonData->currBarrel = (this->localWeaponCommonData->currBarrel + 1) % this->localWeaponCommonData->muzzleLength();
	}
	else
	{
		_globalHitPosition.copy(this->shotResult->hitPoints.last());
		if (this->shotResult->hitPoints.length() == this->shotResult->targets.length())
		{
			_globalHitPosition.subtract(&_globalMuzzlePosition); //.vNormalize().vScale(this->INFINITY).vAdd(this->_globalMuzzlePosition);
			_globalHitPosition.normalize();
			_globalHitPosition.scale(INFINITY);
			_globalHitPosition.add(&_globalMuzzlePosition);
		}

		Vector3 lastHitPoint;
		lastHitPoint.copy(&_globalHitPosition);

		int len = this->shotResult->targets.length();

		if (len > 0)
		{
			if (targets->length() < len)
				for (int i = targets->length(); i < len; i++)
					targets->append(new TerminatorItemHitUser());

			for (int i = 0; i < len; i++)
			{
				TerminatorItemHitUser *item = targets->at(i);
				Vector3 *currHitPoint = this->shotResult->hitPoints.at(i);
				TankData *currTankData = this->shotResult->targets.at(i);
				currTankData->tank->addWorldForceScaled(currHitPoint, &this->shotResult->dir, (double_shoot) ? (commonData->impactForce / 1.5) : commonData->impactForce);
				currHitPoint->subtract(&currTankData->tank->state.pos);
				currHitPoint->transformBy3Tr(&currTankData->tank->baseMatrix);
				item->hitPoint.copy(currHitPoint);
				item->targetPosition.copy(&currTankData->tank->state.pos);
				item->targetIncarnation = currTankData->incarnation;
				item->id = currTankData->user->getId();
			}
		}

		this->battlefieldModel->addDecal(&lastHitPoint, &_barrelOrigin, DECAL_RADIUS, decalMaterial.get(), RotationState::NONE);
		this->fireCommand(targets, len, &lastHitPoint, this->localWeaponCommonData->currBarrel, double_shoot);

		this->localWeaponCommonData->currBarrel = ((this->localWeaponCommonData->currBarrel + 1) % this->localWeaponCommonData->muzzleLength());
	}

	tankData->tank->addWorldForceScaled(&_globalMuzzlePosition, &_globalGunDirection, double_shoot ? (-(commonData->kickback) / 1.5) : -(commonData->kickback));

	IRailgunSFXModel *terminatorSfxModel = WeaponsManager::getTerminatorSFX(tankData->turret);

	Vector3 v;
	v.copy(&_globalHitPosition);
	v.remove(&_globalMuzzlePosition);
	v.normalize();
	double angle = v.cosAngle(&_globalGunDirection);

	IGraphicEffect *graphicEffect;

	if (angle < 0)
	{
		Vector3 v;
		v.copy(&_globalMuzzlePosition);
		v.scale(2);
		v.remove(&_globalHitPosition);
		graphicEffect = terminatorSfxModel->createGraphicShotEffect(tankData->turret, &_globalMuzzlePosition, &v);
	}
	else
	{
		graphicEffect = terminatorSfxModel->createGraphicShotEffect(tankData->turret, &_globalMuzzlePosition, &_globalHitPosition);
	}

	if (graphicEffect != nullptr)
	{
		this->battlefieldModel->addGraphicEffect(graphicEffect);
	}
}

void TerminatorModel::fireCommand(const Vector3 *hitPoint, int currBarrel, bool double_shoot)
{
	QString str;
	str.append("battle;fire;{\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"currBarrel\":");
	str.append(QChar('0' + currBarrel));
	str.append(",\"targetInc\":null,\"double\":");
	str.append(double_shoot ? "true" : "false");
	str.append(",\"targets\":null,\"targetPostitions\":null,\"hitPoints\":[{\"x\":");
	str.append(double_to_string(hitPoint->x));
	str.append(",\"y\":");
	str.append(double_to_string(hitPoint->y));
	str.append(",\"z\":");
	str.append(double_to_string(hitPoint->z));
	str.append("}]}");

	Service::getNetworker()->send(str);
}

void TerminatorModel::fireCommand(QVector<TerminatorItemHitUser*> *targets,
								  int length,
								  const Vector3 *lastHitPoint,
								  int currBarrel,
								  bool double_shoot)
{
	QString str;
	str.append("battle;fire;{\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"currBarrel\":");
	str.append(QChar('0' + currBarrel));
	str.append(",\"targetInc\":[");

	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const TerminatorItemHitUser *item = targets->at(i);
		str.append(QString::number(item->targetIncarnation));
	}

	str.append("],\"double\":");
	str.append(double_shoot ? "true" : "false");
	str.append(",\"targets\":[");

	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const TerminatorItemHitUser *item = targets->at(i);

		str.append(QChar('\"'));
		str.append(item->id);
		str.append(QChar('\"'));
	}

	str.append("],\"targetPostitions\":[");

	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const TerminatorItemHitUser *item = targets->at(i);

		str.append("{\"x\":");
		str.append(double_to_string(item->targetPosition.x));
		str.append(",\"y\":");
		str.append(double_to_string(item->targetPosition.y));
		str.append(",\"z\":");
		str.append(double_to_string(item->targetPosition.z));
		str.append("}");
	}

	str.append("],\"hitPoints\":[");

	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const TerminatorItemHitUser *item = targets->at(i);

		str.append("{\"x\":");
		str.append(double_to_string(item->hitPoint.x));
		str.append(",\"y\":");
		str.append(double_to_string(item->hitPoint.y));
		str.append(",\"z\":");
		str.append(double_to_string(item->hitPoint.z));
		str.append("}");
	}

	if (length != 0)
		str.append(QChar(','));

	str.append("{\"x\":");
	str.append(double_to_string(lastHitPoint->x));
	str.append(",\"y\":");
	str.append(double_to_string(lastHitPoint->y));
	str.append(",\"z\":");
	str.append(double_to_string(lastHitPoint->z));
	str.append("}]}");

	Service::getNetworker()->send(str);
}
