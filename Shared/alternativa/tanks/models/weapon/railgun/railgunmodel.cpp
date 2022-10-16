#include "railgunmodel.h"
#include "RailgunData.h"
#include "alternativa/object/clientobjectweaponrailgun.h"
#include "service.h"
#include "railguntargetsystem.h"
#include "RailgunShotResult.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "service.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "../common/IWeaponCommonModel.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "../common/weaponcommondata.h"
#include "alternativa/tanks/models/sfx/shoot/railgun/railgunsfxmodel.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/physics/physicsscene.h"
#include "RailgunShotResult.h"
#include "battlecontroller.h"
#include "resource/resourceutil.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/models/battlefield/decals/RotationState.h"
#include "networking/network.h"
#include "double_to_string.h"
#include "alternativa/tanks/models/weapon/WeaponConst.h"


static const double DECAL_RADIUS = 50;

static double const INFINITY = 20000;

static RailgunData* getRailgunData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponRailgun::data(clientObject);
}


class RailgunItemHitUser
{
public:
	QString id;
	Vector3 targetPosition;
	Vector3 hitPoint;
	int targetIncarnation;
};



RailgunModel::RailgunModel()
{
	this->targetSystem = new RailgunTargetSystem();
	this->shotResult = new RailgunShotResult();
	this->firstshot = true;
	this->commonModel = nullptr;
	this->_triggerPressed = false;
	this->chargeTimeLeft = 0;
	this->nextReadyTime = 0;
	this->firstshot = true;

	this->targets = new QVector<RailgunItemHitUser*>;

#ifdef GRAPHICS
	decalMaterial = new_TextureMaterial();
	decalMaterial->init();
	decalMaterial->setTexture(ResourceUtil::getResourceImageLocal("decal/railgun.png"));
#endif
}

RailgunModel::~RailgunModel()
{
	qDeleteAll(*targets);
	delete targets;
}

void RailgunModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
	if (this->commonModel == nullptr)
	{
		this->battlefieldModel = Service::getBattlefieldModel();
		this->tankModel = Service::getITank();
		this->commonModel = Service::getIWeaponCommonModel();
	}
}

void RailgunModel::objectUnloaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
}

void RailgunModel::initObject(ClientObjectWeapon *clientObject, int chargingTimeMsec, double weakeningCoeff)
{
	std::shared_ptr<RailgunData> railgunData = std::make_shared<RailgunData>();
	railgunData->chargingTime = chargingTimeMsec;
	railgunData->weakeningCoeff = weakeningCoeff;
	ClientObjectWeaponRailgun::setData(clientObject, railgunData);
	//this->battlefieldModel.blacklist.push(decalMaterial.getTextureResource());
	this->objectLoaded(clientObject);
}

void RailgunModel::startFire(ClientObjectUser *clientObject, const QString &firingTankId)
{
	(void)firingTankId;

#ifdef GRAPHICS
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
	if ((((firingTankData->tank == nullptr) || (!(firingTankData->enabled))) || (firingTankData->local)))
	{
		return;
	}

	if (this->commonModel == nullptr)
	{
		this->commonModel = Service::getIWeaponCommonModel();
	}

	WeaponCommonData *commonData = this->commonModel->getCommonData(firingTankData->turret);

	Vector3 _globalMuzzlePosition;
	Vector3 _globalGunDirection;


	WeaponUtils::calculateGunParamsAux(firingTankData->tank->skin->turretMesh,
									   commonData->getMuzzle(0),
									   &_globalMuzzlePosition,
									   &_globalGunDirection);

	IRailgunSFXModel *railgunSfxModel = WeaponsManager::getRailgunSFX(firingTankData->turret);
	RailgunData *railgunData = getRailgunData(firingTankData->turret);

	RailgunShootSFXData *swfData = ClientObjectWeaponRailgun::SWFData(firingTankData->turret);


	IGraphicEffect *graphicEffect = railgunSfxModel->createChargeEffect(firingTankData->turret,
																		firingTankData->user,
																		swfData,
																		commonData->getMuzzle(commonData->currBarrel),
																		firingTankData->tank->skin->turretMesh,
																		railgunData->chargingTime);

	if (this->battlefieldModel == nullptr)
	{
		this->battlefieldModel = Service::getBattlefieldModel();
	}

	if (this->firstshot == true)
	{
		this->firstshot = false;
		this->battlefieldModel->addGraphicEffect(graphicEffect);
		graphicEffect = railgunSfxModel->createChargeEffect(firingTankData->turret,
															firingTankData->user,
															swfData,
															commonData->getMuzzle(commonData->currBarrel),
															firingTankData->tank->skin->turretMesh, railgunData->chargingTime);
	}

	this->battlefieldModel->addGraphicEffect(graphicEffect);
	//ISound3DEffect *soundEffect = railgunSfxModel.createSoundShotEffect(firingTankData.turret, firingTankData.user, this->_globalMuzzlePosition);
	//if (soundEffect != nullptr)
	//{
	//	this->battlefieldModel.addSound3DEffect(soundEffect);
	//}
#else
	(void)clientObject;
#endif
}



void RailgunModel::fire(ClientObjectUser *clientObject, QVector<Vector3*> &affectedPoints, QStringList &affectedTankIds)
{	
	ClientObjectUser *firingTank = clientObject;
	if (firingTank == nullptr)
		return;

	TankData *firingTankData = this->tankModel->getTankData(firingTank);
	if ((((firingTankData == nullptr) || (firingTankData->tank == nullptr)) || (!(firingTankData->enabled))) || (firingTankData->local))
	{
		return;
	}

	if (this->commonModel == nullptr)
	{
		this->commonModel = Service::getIWeaponCommonModel();
	}

	if (this->battlefieldModel == nullptr)
		this->battlefieldModel = Service::getBattlefieldModel();

	WeaponCommonData *commonData = this->commonModel->getCommonData(firingTankData->turret);
	RailgunData *railGunData = getRailgunData(firingTankData->turret);
	Vector3 _globalMuzzlePosition;
	Vector3 _globalGunDirection;
	WeaponUtils::calculateGunParamsAux(firingTankData->tank->skin->turretMesh,
									   commonData->getMuzzle(0),
									   &_globalMuzzlePosition,
									   &_globalGunDirection);
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
					Vector3 _globalHitPosition;
					Vector3 *v = affectedPoints[i];
					_globalHitPosition.x = v->x;
					_globalHitPosition.y = v->y;
					_globalHitPosition.z = v->z;
					_globalHitPosition.transformBy3(&affectedTankData->tank->baseMatrix);
					_globalHitPosition.add(&affectedTankData->tank->state.pos);
					affectedTankData->tank->addWorldForceScaled(&_globalHitPosition, &_globalGunDirection, impactForce);
					this->battlefieldModel->tankHit(affectedTankData, &_globalGunDirection, (impactForce / WeaponConst::BASE_IMPACT_FORCE));
					impactForce = impactForce * railGunData->weakeningCoeff;
				}
			}
			i++;
		}
	}
	Vector3 *v = affectedPoints.at(numPoints - 1);
	Vector3 _globalHitPosition;
	_globalHitPosition.x = v->x;
	_globalHitPosition.y = v->y;
	_globalHitPosition.z = v->z;
	this->battlefieldModel->addDecal(&_globalHitPosition, &_globalMuzzlePosition, DECAL_RADIUS, decalMaterial.get(), RotationState::NONE);
	firingTankData->tank->addWorldForceScaled(&_globalMuzzlePosition, &_globalGunDirection, -commonData->kickback);

#ifdef GRAPHICS
	IRailgunSFXModel *railgunSfxModel = WeaponsManager::getRailgunSFX(firingTankData->turret);

	Vector3 v2;
	v2.copy(&_globalHitPosition);
	v2.remove(&_globalMuzzlePosition);
	v2.normalize();
	double angle =  v2.cosAngle(&_globalGunDirection);


	IGraphicEffect *graphicEffect;
	if (angle < 0)
	{
		Vector3 v;
		v.copy(&_globalMuzzlePosition);
		v.scale(2);
		v.remove(&_globalHitPosition);
		graphicEffect = railgunSfxModel->createGraphicShotEffect(firingTankData->turret, &_globalMuzzlePosition, &v);
	}
	else
	{
		graphicEffect = railgunSfxModel->createGraphicShotEffect(firingTankData->turret, &_globalMuzzlePosition, &_globalHitPosition);
	}
	if (graphicEffect != nullptr)
	{
		this->battlefieldModel->addGraphicEffect(graphicEffect);
	}
#endif
}

void RailgunModel::activateWeapon(int time)
{
	(void)time;
	this->_triggerPressed = true;
}

void RailgunModel::deactivateWeapon(int time, bool sendServerCommand)
{
	(void)time;
	(void)sendServerCommand;
	this->_triggerPressed = false;
}


void RailgunModel::setLocalUser(TankData *localUserData)
{
	this->localTankData = localUserData;
	this->localShotData = WeaponsManager::shotData(localUserData->turret->getId());
	this->localRailgunData = getRailgunData(localUserData->turret);
	this->localWeaponCommonData = this->commonModel->getCommonData(localUserData->turret);
	ICTFModel *ctfModel = nullptr;
	this->battlefieldModel = Service::getBattlefieldModel();
	this->targetSystem->setParams(this->battlefieldModel->getBattlefieldData()->physicsScene->collisionDetector,
								  this->localShotData->autoAimingAngleUp,
								  this->localShotData->numRaysUp,
								  this->localShotData->autoAimingAngleDown,
								  this->localShotData->numRaysDown,
								  this->localRailgunData->weakeningCoeff,
								  ctfModel);
	this->reset();


#ifdef GRAPHICS
	const Vector3 *muzzleLocalPos = this->localWeaponCommonData->getMuzzle(0);
	IRailgunSFXModel *railgunSfxModel = WeaponsManager::getRailgunSFX(this->localTankData->turret);
	RailgunShootSFXData *swfData = ClientObjectWeaponRailgun::SWFData(localTankData->turret);

	IGraphicEffect *graphicEffect = railgunSfxModel->createChargeEffect(this->localTankData->turret,
																		this->localTankData->user,
																		swfData,
																		muzzleLocalPos,
																		this->localTankData->tank->skin->turretMesh,
																		this->localRailgunData->chargingTime);

	if (graphicEffect != nullptr)
	{
		this->battlefieldModel->addGraphicEffect(graphicEffect);
	}
#endif
}

void RailgunModel::clearLocalUser()
{
	this->localTankData = nullptr;
	this->localShotData = nullptr;
	this->localRailgunData = nullptr;
	this->localWeaponCommonData = nullptr;
}


double RailgunModel::update(int time, int deltaTime)
{
	if (this->chargeTimeLeft > 0)
	{
		this->chargeTimeLeft = this->chargeTimeLeft - deltaTime;
		if (this->chargeTimeLeft <= 0)
		{
			this->chargeTimeLeft = 0;
			this->doFire(this->localWeaponCommonData, this->localTankData, time);
		}
		return (double)this->chargeTimeLeft / (double)this->localRailgunData->chargingTime;
	}
	if (time < this->nextReadyTime)
	{
		return 1. - (((double)this->nextReadyTime - time) / (double)this->localShotData->reloadMsec);
	}
	if (this->_triggerPressed)
	{
		this->chargeTimeLeft = this->localRailgunData->chargingTime;

#ifdef GRAPHICS
		const Vector3 *muzzleLocalPos = this->localWeaponCommonData->getMuzzle(0);

		Vector3 _globalMuzzlePosition;
		Vector3 _barrelOrigin;
		Vector3 _xAxis;
		Vector3 _globalGunDirection;

		WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh,
										muzzleLocalPos,
										&_globalMuzzlePosition,
										&_barrelOrigin,
										&_xAxis,
										&_globalGunDirection);

		IRailgunSFXModel *railgunSfxModel = WeaponsManager::getRailgunSFX(this->localTankData->turret);

		RailgunShootSFXData *swfData = ClientObjectWeaponRailgun::SWFData(localTankData->turret);

		IGraphicEffect *graphicEffect = railgunSfxModel->createChargeEffect(this->localTankData->turret,
																			this->localTankData->user,
																			swfData,
																			muzzleLocalPos,
																			this->localTankData->tank->skin->turretMesh,
																			this->localRailgunData->chargingTime);
		if (graphicEffect != nullptr)
		{
			this->battlefieldModel->addGraphicEffect(graphicEffect);
		}
#endif

		//ISound3DEffect *soundEffect = railgunSfxModel.createSoundShotEffect(this->localTankData.turret, this->localTankData.user, this->_globalMuzzlePosition);
		//if (soundEffect != null)
			//this->battlefieldModel.addSound3DEffect(soundEffect);

		this->startFireCommand();
	}

	return 1;
}

void RailgunModel::startFireCommand()
{
	Service::getNetworker()->send("battle;start_fire");
}

void RailgunModel::reset()
{
	this->nextReadyTime = this->chargeTimeLeft = 0;
	this->_triggerPressed = false;
}

void RailgunModel::stopEffects(TankData *ownerTankData)
{
	(void)ownerTankData;
}



void RailgunModel::doFire(WeaponCommonData *commonData, TankData *tankData, int time)
{
	this->nextReadyTime = time + this->localShotData->reloadMsec;

	Vector3 _globalMuzzlePosition;
	Vector3 _barrelOrigin;
	Vector3 _xAxis;
	Vector3 _globalGunDirection;

	WeaponUtils::calculateGunParams(tankData->tank->skin->turretMesh,
									commonData->getMuzzle(commonData->currBarrel),
									&_globalMuzzlePosition,
									&_barrelOrigin,
									&_xAxis,
									&_globalGunDirection);

	Vector3 _globalHitPosition;

	BattlefieldData *bfData = this->battlefieldModel->getBattlefieldData();

	QHash<Body*, TankData*> *tanks = (QHash<Body*, TankData*>*)bfData->tanks;

	this->targetSystem->getTargets(tankData, &_barrelOrigin, &_globalGunDirection, &_xAxis, tanks, this->shotResult);


	if (this->shotResult->hitPoints.length() == 0)
	{
		Vector3 hitPos3d;
		hitPos3d.x = _globalMuzzlePosition.x + (INFINITY * _globalGunDirection.x);
		hitPos3d.y = _globalMuzzlePosition.y + (INFINITY * _globalGunDirection.y);
		hitPos3d.z = _globalMuzzlePosition.z + (INFINITY * _globalGunDirection.z);
		_globalHitPosition.copy(&hitPos3d);
		this->fireCommand(&hitPos3d);
	}
	else
	{
		_globalHitPosition.copy(this->shotResult->hitPoints.last());
		if (this->shotResult->hitPoints.length() == this->shotResult->targets.length())
		{
			_globalHitPosition.subtract(&_globalMuzzlePosition);
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
					targets->append(new RailgunItemHitUser());

			for (int i = 0; i < len; i++)
			{
				RailgunItemHitUser *item = targets->at(i);
				Vector3 *currHitPoint = this->shotResult->hitPoints.at(i);
				const TankData *currTankData = this->shotResult->targets.at(i);
				currTankData->tank->addWorldForceScaled(currHitPoint, &this->shotResult->dir, commonData->impactForce);
				currHitPoint->subtract(&currTankData->tank->state.pos);
				currHitPoint->transformBy3Tr(&currTankData->tank->baseMatrix);
				item->hitPoint.copy(currHitPoint);
				item->targetPosition.copy(&currTankData->tank->state.pos);
				item->targetIncarnation = currTankData->incarnation;
				item->id = currTankData->user->getId();
			}
		}

		this->battlefieldModel->addDecal(&lastHitPoint, &_barrelOrigin, DECAL_RADIUS, decalMaterial.get(), RotationState::NONE);

		this->fireCommand(targets, len, &lastHitPoint);
	}

	tankData->tank->addWorldForceScaled(&_globalMuzzlePosition, &_globalGunDirection, -commonData->kickback);

#ifdef GRAPHICS
	IRailgunSFXModel *railgunSfxModel = WeaponsManager::getRailgunSFX(tankData->turret);

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

		graphicEffect = railgunSfxModel->createGraphicShotEffect(tankData->turret,
																 &_globalMuzzlePosition,
																 &v);
	}
	else
	{
		graphicEffect = railgunSfxModel->createGraphicShotEffect(tankData->turret, &_globalMuzzlePosition, &_globalHitPosition);
	}
	if (graphicEffect != nullptr)
	{
		this->battlefieldModel->addGraphicEffect(graphicEffect);
	}
#endif
}


void RailgunModel::fireCommand(const Vector3 *hitPoints)
{
	QString str;
	str.append("battle;fire;{\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"targets\":null,\"targetPostitions\":null,\"hitPoints\":[{\"y\":");
	str.append(double_to_string(hitPoints->y));
	str.append(",\"z\":");
	str.append(double_to_string(hitPoints->z));
	str.append(",\"x\":");
	str.append(double_to_string(hitPoints->x));
	str.append("}],\"targetInc\":null}");
	Service::getNetworker()->send(str);
}


void RailgunModel::fireCommand(QVector<RailgunItemHitUser*> *targets, int length, const Vector3 *lastHitPoint)
{
	QString str;
	str.append("battle;fire;{\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"targets\":[");


	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const RailgunItemHitUser *item = targets->at(i);

		str.append("\"");
		str.append(item->id);
		str.append("\"");
	}

	str.append("],\"targetPostitions\":[");

	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const RailgunItemHitUser *item = targets->at(i);

		str.append("{\"y\":");
		str.append(double_to_string(item->targetPosition.y));
		str.append(",\"z\":");
		str.append(double_to_string(item->targetPosition.z));
		str.append(",\"x\":");
		str.append(double_to_string(item->targetPosition.x));
		str.append("}");
	}

	str.append("],\"hitPoints\":[");

	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const RailgunItemHitUser *item = targets->at(i);

		str.append("{\"y\":");
		str.append(double_to_string(item->hitPoint.y));
		str.append(",\"z\":");
		str.append(double_to_string(item->hitPoint.z));
		str.append(",\"x\":");
		str.append(double_to_string(item->hitPoint.x));
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

	str.append("}],\"targetInc\":[");

	for (int i = 0; i < length; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const RailgunItemHitUser *item = targets->at(i);
		str.append(QString::number(item->targetIncarnation));
	}

	str.append("]}");

	Service::getNetworker()->send(str);
}
