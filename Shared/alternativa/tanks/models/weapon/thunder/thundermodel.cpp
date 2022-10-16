#include "thundermodel.h"
#include "thunderdata.h"
#include "../WeaponConst.h"
#include "alternativa/object/clientobjectweaponthunder.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "_global.h"
#include "resource/resourceutil.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/common/IWeaponCommonModel.h"
#include "alternativa/tanks/models/battlefield/decals/RotationState.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/weapon/shared/commontargetevaluator.h"
#include "alternativa/tanks/models/weapon/shared/commontargetsystem.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "mymath.h"
#include "alternativa/tanks/models/weapon/common/HitInfo.h"
#include "alternativa/tanks/models/weapon/weakening/weaponweakeningmodel.h"
#include "alternativa/object/clientobjectuser.h"
#include "service.h"
#include "battlecontroller.h"
#include "alternativa/tanks/sfx/effectspair.h"
#include "alternativa/tanks/models/sfx/shoot/thunder/IThunderSFXModel.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "networking/network.h"
#include "double_to_string.h"
#include <QString>


static const double DECAL_RADIUS = 250;

namespace
{

	class Hit
	{
	public:
		QString id; //+
		int incarnation;
		double distance; //+
		Vector3 position;

		Hit() {}
		Hit(const Hit &other)
		{
			this->id = other.id;
			this->incarnation = other.incarnation;
			this->distance = other.distance;
			this->position.copy(&other.position);
		}

		void operator=(const Hit &other)
		{
			this->id = other.id;
			this->incarnation = other.incarnation;
			this->distance = other.distance;
			this->position.copy(&other.position);
		}
	};

	void fireCommand(ShotData *localShotData,
					 const Vector3 *hitPos,
					 const QString &mainTargetId,
					 int mainTargetIncarnation,
					 const Vector3 *mainTargetPosition,
					 int distance,
					 const QList<Hit> &hits)
	{
		(void)mainTargetIncarnation;

		QString str;

		str.append("battle;fire;{\"splashTargetIds\":");

		if (hits.isEmpty())
			str.append("null");
		else {
			int first = true;
			for (const Hit &hit : hits)
			{
				if (first == false)
					str.append(", ");
				first = false;
				str.append("[\"" + hit.id + "\"]");
			}
		}

		if (hitPos == nullptr)
		{
			str.append(",\"hitPos\":null,");
		}
		else
		{
			str.append(",\"hitPos\":{\"y\":");
			str.append(double_to_string(hitPos->y));
			str.append(",\"z\":");
			str.append(double_to_string(hitPos->z));
			str.append(",\"x\":");
			str.append(double_to_string(hitPos->x));
			str.append("},");
		}

		if (mainTargetPosition == nullptr)
		{
			str.append("\"mainTargetPosition\":null,");
		}
		else
		{
			str.append("\"mainTargetPosition\":{\"y\":");
			str.append(double_to_string(hitPos->y));
			str.append(",\"z\":");
			str.append(double_to_string(hitPos->z));
			str.append(",\"x\":");
			str.append(double_to_string(hitPos->x));
			str.append("},");
		}

		str.append("\"reloadTime\":");
		str.append(QString::number(localShotData->reloadMsec));
		str.append(",");

		if (hits.isEmpty())
			str.append("\"splashTargetDistances\":null,");
		else {
			str.append("\"splashTargetDistances\":[");

			int first = true;
			for (const Hit &hit : hits)
			{
				if (first == false)
					str.append(", ");
				first = false;
				str.append(double_to_string(hit.distance));
			}
			str.append("],");
		}

		str.append("\"distance\":");
		str.append(QString::number(distance));

		if (mainTargetId == nullptr || mainTargetId.isEmpty())
			str.append(",\"mainTargetId\":null}");
		else
			str.append(",\"mainTargetId\":\"" + mainTargetId + "\"}");

		Service::getNetworker()->send(str);

		/*
		var js:Object = new Object();
		js.hitPos = hitPos;
		js.mainTargetId = mainTargetId;
		js.mainTargetPosition = mainTargetPosition;
		js.distance = distance;
		js.splashTargetIds = splashTargetIds;
		js.splashTargetDistances = splashTargetDistances;
		js.reloadTime = this->localShotData.reloadMsec.value;
		Network(Main.osgi.getService(INetworker)).send(("battle;fire;" + JSON.stringify(js)));
		*/
	}
}


static ThunderData* getData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponThunder::data(clientObject);
}

ThunderModel::ThunderModel()
{
	this->commonModel = nullptr;
	this->battlefieldModel = nullptr;
	this->tankInterface = nullptr;
	this->sfxModel = nullptr;
	this->localThunderData = nullptr;
#ifdef GRAPHICS
	decalMaterial = new_TextureMaterial();
	decalMaterial->init();
	decalMaterial->setTexture(ResourceUtil::getResourceImageLocal("decal/thunder.png"));
#endif
}

ThunderModel::~ThunderModel()
{

}


void ThunderModel::initObject(ClientObjectWeapon *clientObject, double impactForce, double maxSplashDamageRadius, double minSplashDamagePercent, double minSplashDamageRadius)
{
	std::shared_ptr<ThunderData> data = std::make_shared<ThunderData>(maxSplashDamageRadius, minSplashDamageRadius, minSplashDamagePercent, (impactForce * WeaponConst::BASE_IMPACT_FORCE));
	ClientObjectWeaponThunder::setData(clientObject, data);

#ifdef GRAPHICS
	WeaponsManager::getThunderSFX(WeaponsManager::getObjectFor(clientObject->getId()));
#endif
	this->objectLoaded(clientObject);
}


void ThunderModel::fire(ClientObjectUser *clientObject,
						const QString &shooterId,
						const Vector3 *hitPoint,
						const QString &mainTargetId,
						double weakeningCoeff,
						QVector<QString> &splashTargetIds)
{
	(void)clientObject;;

	TankData *shooterData = this->getTankData(shooterId);
	if ((((shooterData == nullptr) || (shooterData->tank == nullptr)) || (shooterData->tank->skin == nullptr)))
	{
		return;
	}

	if (this->tankInterface->localUserData != nullptr)
	{
		if (*shooterData->userName == *this->tankInterface->localUserData->userName)
		{
			return;
		}
	}

	WeaponCommonData *commonData = this->commonModel->getCommonData(shooterData->turret);
	Vector3 _muzzlePosGlobal;
	int barrel = 0;

	const Vector3 *muzzle = commonData->getMuzzle(barrel);

	WeaponUtils::calculateGunParamsAux(shooterData->tank->skin->turretMesh,
									   muzzle,
									   &_muzzlePosGlobal,
									   &this->_gunDirGlobal);

	this->createShotEffects(shooterData, muzzle);

	Vector3 _vector;
	_vector.copy(&this->_gunDirGlobal);
	_vector.scale(-commonData->kickback);

	shooterData->tank->addWorldForce(&_muzzlePosGlobal, &_vector);

	if (hitPoint == nullptr)
		return;

	Vector3 _hitPos;
	_hitPos.x = hitPoint->x;
	_hitPos.y = hitPoint->y;
	_hitPos.z = hitPoint->z;

	ThunderData *thunderData = getData(shooterData->turret);
	if (mainTargetId != nullptr)
	{
		TankData *mainTargetData = this->getTankData(mainTargetId);
		if (((mainTargetData == nullptr) || (mainTargetData->tank == nullptr)))
		{
			return;
		}
		mainTargetData->tank->baseMatrix.transformVector(&_hitPos, &_vector);
		_hitPos.sum(&_vector, &mainTargetData->tank->state.pos);
		this->applyMainImpactForce(mainTargetData->tank, &_hitPos, &this->_gunDirGlobal, commonData->impactForce, thunderData->impactForce, weakeningCoeff);
		this->battlefieldModel->tankHit(mainTargetData, &this->_gunDirGlobal, commonData->impactCoeff * weakeningCoeff);
	}
	else
	{
		this->battlefieldModel->addDecal(&_hitPos, &this->_barrelOrigin, DECAL_RADIUS, decalMaterial.get(), RotationState::NONE);
	}

	this->createExplosionEffects(shooterData, &_hitPos);
	double impactCoeff = thunderData->impactForce / WeaponConst::BASE_IMPACT_FORCE;
	if (!splashTargetIds.isEmpty())
	{
		int len = splashTargetIds.length();
		for (int i = 0; i < len; i++)
		{
			TankData *targetData = this->getTankData(splashTargetIds.at(i));
			if (!((targetData == nullptr) || (targetData->tank == nullptr)))
			{
				Vector3 _vector;
				_vector.diff(&targetData->tank->state.pos, &_hitPos);
				_vector.normalize();
				double splashWeakeningCoeff = 1;
				this->battlefieldModel->tankHit(targetData, &_vector, splashWeakeningCoeff * impactCoeff);
				_vector.scale(thunderData->impactForce * splashWeakeningCoeff);
				targetData->tank->addForce(&_vector);
			}
		}
	}
}

void ThunderModel::stopEffects(TankData *ownerTankData)
{
	(void)ownerTankData;
}

void ThunderModel::reset()
{
	this->_triggerPressed = false;
	this->nextReadyTime = 0;
}


void ThunderModel::setLocalUser(TankData *localUserData)
{
	this->localTankData = localUserData;
	this->localShotData = WeaponsManager::shotData(localUserData->turret->getId());
	this->localThunderData = ClientObjectWeaponThunder::data(localUserData->turret);
	this->localWeaponCommonData = this->commonModel->getCommonData(localUserData->turret);

	this->targetEvaluator = CommonTargetEvaluator::create(this->localTankData,
														  this->localShotData,
														  this->battlefieldModel,
														  true,
														  nullptr);

	this->targetSystem = new CommonTargetSystem(this->maxTargetingDistance,
												this->localShotData->autoAimingAngleUp,
												this->localShotData->numRaysUp,
												this->localShotData->autoAimingAngleDown,
												this->localShotData->numRaysDown,
												this->battlefieldModel->getBattlefieldData()->collisionDetector,
												this->targetEvaluator);
}

void ThunderModel::clearLocalUser()
{
	this->localTankData = nullptr;
	this->localShotData = nullptr;
	this->localWeaponCommonData = nullptr;
	this->targetSystem = nullptr;
	this->targetEvaluator = nullptr;
}

void ThunderModel::activateWeapon(int time)
{
	(void)time;
	this->_triggerPressed = true;
}

void ThunderModel::deactivateWeapon(int time, bool sendServerCommand)
{
	(void)time;
	(void)sendServerCommand;
	this->_triggerPressed = false;
}



double ThunderModel::update(int time, int deltaTime)
{
	(void)deltaTime;

	TankData *mainTarget = nullptr;
	//int numSplashTargets = 0;



	//var splashTargetIds:Array;
	//var splashTargetIncarnations:Array;
	//var splashTargetDistances:Array;
	//var splashTargetPositions:Array;



	QList<Hit> hits;


	//var v:Vector3;
	if (!this->_triggerPressed || time < this->nextReadyTime)
	{
		if (time < this->nextReadyTime)
		{
			return 1. + (double(time - this->nextReadyTime) / double(this->localShotData->reloadMsec));
		}
		return 1;
	}

	this->nextReadyTime = (time + this->localShotData->reloadMsec);

	Vector3 _muzzlePosGlobal;
	Vector3 _xAxis;
	Vector3 _vector;

	const Vector3 *muzzle = this->localWeaponCommonData->getMuzzle(0);


	WeaponUtils::calculateGunParams(
				this->localTankData->tank->skin->turretMesh,
				muzzle,
				&_muzzlePosGlobal,
				&this->_barrelOrigin,
				&_xAxis,
				&this->_gunDirGlobal);

	_vector.copy(&this->_gunDirGlobal);
	_vector.scale(-this->localWeaponCommonData->kickback);

	this->localTankData->tank->addWorldForce(&_muzzlePosGlobal, &_vector);

	this->createShotEffects(this->localTankData, muzzle);
	BattlefieldData *bfData = this->battlefieldModel->getBattlefieldData();
	ICollisionDetector *collisionDetector = bfData->collisionDetector;
	HitInfo hitInfo;

	if (this->targetSystem->getTarget(&this->_barrelOrigin, &this->_gunDirGlobal, &_xAxis, this->localTankData->tank, &hitInfo))
	{
		double distance = hitInfo.distance * 0.01;
		double directImpactCoeff = WeaponWeakeningModel::getImpactCoeff(this->localTankData->turret, distance);
		double hitX = hitInfo.position.x;
		double hitY = hitInfo.position.y;
		double hitZ = hitInfo.position.z;
		double splashRadiusSqr = (10000 * this->localThunderData->minSplashDamageRadius) * this->localThunderData->minSplashDamageRadius;

		QHashIterator<TankData*, bool> i(*bfData->activeTanks);
		while (i.hasNext())
		{
			i.next();

			TankData *td = i.key();
			Tank *tank = td->tank;
			if (td->spawnState == TankSpawnState::NEWCOME)
			{
			}
			else
			{
				if (tank == hitInfo.body)
				{
					mainTarget = td;
				}
				else
				{
					const Vector3 *targetPos = &tank->state.pos;
					double dx = targetPos->x - hitX;
					double dy = targetPos->y - hitY;
					double dz = targetPos->z - hitZ;
					double d = (dx * dx) + (dy * dy) + (dz * dz);
					if (d <= splashRadiusSqr)
					{
						if (this->testForSplashHit(tank, &hitInfo, collisionDetector))
						{
							d = MyMath::sqrt(d) * 0.01;

							Hit hit;
							hit.id = td->user->getId();;
							hit.incarnation = td->incarnation;
							hit.distance = d;
							hit.position.copy(targetPos);

							//this->_splashTargetIds[numSplashTargets] = td->user->getId();
							//this->_splashTargetIncarnations[numSplashTargets] = td->incarnation;
							//this->_splashTargetDistances[numSplashTargets] = d;


							//this->_splashTargetIds.append(td->user->getId());
							//this->_splashTargetIncarnations.append(td->incarnation);
							//this->_splashTargetDistances.append(d);

							/*
							Vector3 *targetPos3d = this->_splashTargetPositions.at(numSplashTargets);
							if (targetPos3d == nullptr)
							{
								targetPos3d = new Vector3(0, 0, 0);
								this->_splashTargetPositions[numSplashTargets] = targetPos3d;
							}
							*/

							hits.append(hit);

							//targetPos3d->x = targetPos->x;
							//targetPos3d->y = targetPos->y;
							//targetPos3d->z = targetPos->z;
							//numSplashTargets++;
							_vector.normalize();
							_vector.scale((this->localThunderData->impactForce * directImpactCoeff) * this->getSplashImpactCoeff(d));
							tank->addForce(&_vector);
						};
					};
				};
			};
		};

		QString mainTargetId;
		int mainTargetIncarnation = 0;
		Vector3 hitPos3d;// = this->_hitPos3d;
		Vector3 *mainTargetPosition;
		Vector3 _targetPos3d;


		if (mainTarget != nullptr)
		{
			mainTargetId = mainTarget->user->getId();
			mainTargetIncarnation = mainTarget->incarnation;
			mainTargetPosition = &_targetPos3d;
			const Vector3 *v = &mainTarget->tank->state.pos;
			mainTargetPosition->copy(v);
			this->applyMainImpactForce(mainTarget->tank, &hitInfo.position, &this->_gunDirGlobal,
									   this->localWeaponCommonData->impactForce, this->localThunderData->impactForce, directImpactCoeff);
			Vector3 _hitPosGlobal;
			_hitPosGlobal.diff(&hitInfo.position, v);
			Vector3 _hitPosLocal;
			mainTarget->tank->baseMatrix.transformVectorInverse(&_hitPosGlobal, &_hitPosLocal);
			hitPos3d.x = _hitPosLocal.x;
			hitPos3d.y = _hitPosLocal.y;
			hitPos3d.z = _hitPosLocal.z;
		}
		else
		{
			hitPos3d.x = hitX;
			hitPos3d.y = hitY;
			hitPos3d.z = hitZ;
		}

		/*
		if (numSplashTargets > 0)
		{
			splashTargetIds = this->_splashTargetIds;
			splashTargetIds.length = numSplashTargets;

			splashTargetIncarnations = this->_splashTargetIncarnations;
			splashTargetIncarnations.length = numSplashTargets;

			splashTargetDistances = this->_splashTargetDistances;
			splashTargetDistances.length = numSplashTargets;

			splashTargetPositions = this->_splashTargetPositions;
			splashTargetPositions.length = numSplashTargets;
		}
		*/

		::fireCommand(localShotData,
					  &hitPos3d,
					  mainTargetId,
					  mainTargetIncarnation,
					  mainTargetPosition,
					  distance,
					  hits);

		this->createExplosionEffects(this->localTankData, &hitInfo.position);
		if (mainTarget == nullptr)
		{
			this->battlefieldModel->addDecal(&hitInfo.position, &this->_barrelOrigin, DECAL_RADIUS, decalMaterial.get(), RotationState::NONE);
		}
	}
	else
	{
		fireCommand(localShotData, nullptr, nullptr, 0, nullptr, 0, hits);
	}

	return 0;
}


void ThunderModel::objectLoaded(ClientObjectWeapon *object)
{
	if (this->commonModel == nullptr)
	{
		this->battlefieldModel = Service::getIBattleField();
		this->tankInterface = Service::getTankModel();
		this->commonModel = Service::getIWeaponCommonModel();
#ifdef GRAPHICS
		this->sfxModel = WeaponsManager::getThunderSFX(WeaponsManager::getObjectFor(object->getId()));
#else
		(void)object;
#endif
	}
	this->nextReadyTime = 0;
}

void ThunderModel::objectUnloaded(ClientObject *object)
{
	(void)object;
}

double ThunderModel::getSplashImpactCoeff(double distance)
{
	if (distance < this->localThunderData->maxSplashDamageRadius)
	{
		return 1;
	}
	return 0.01 * (this->localThunderData->minSplashDamagePercent +
				   (((this->localThunderData->minSplashDamageRadius - distance) * (100 - this->localThunderData->minSplashDamagePercent))
					/ (this->localThunderData->minSplashDamageRadius - this->localThunderData->maxSplashDamageRadius)));
}

TankData *ThunderModel::getTankData(const QString &objectId)
{
	ClientObjectUser *object = Service::getBattleController()->tanks->value(objectId);
	if (object == nullptr)
		return nullptr;
	return this->tankInterface->getTankData(object);
}

void ThunderModel::createShotEffects(TankData *tankData, const Vector3 *muzzlePosLocal)
{
#ifdef GRAPHICS
	EffectsPair effects;
	this->sfxModel->createShotEffects(
				WeaponsManager::getObjectFor(tankData->turret->getId()),
				muzzlePosLocal, tankData->tank->skin->turretMesh,
				&effects);

	this->battlefieldModel->addGraphicEffect(effects.graphicEffect);
	//this->battlefieldModel.addSound3DEffect(effects.soundEffect);
#else
	(void)tankData;
	(void)muzzlePosLocal;
#endif
}

void ThunderModel::createExplosionEffects(TankData *tankData, const Vector3 *position)
{
#ifdef GRAPHICS
	EffectsPair effects;
	this->sfxModel->createExplosionEffects(tankData->turret, position, &effects);
	this->battlefieldModel->addGraphicEffect(effects.graphicEffect);
	//this->battlefieldModel.addSound3DEffect(effect.soundEffect);
#else
	(void)tankData;
	(void)position;
#endif
}


bool ThunderModel::testForSplashHit(Tank *tank, HitInfo *hitInfo, ICollisionDetector *collisionDetector)
{
	double offset = 0.75 * tank->mainCollisionBox->hs.y;

	Vector3 testPointsAux[3];

	testPointsAux[0].x = 0;
	testPointsAux[0].y = -offset;
	testPointsAux[0].z = 0;

	testPointsAux[1].x = 0;
	testPointsAux[1].y = 0;
	testPointsAux[1].z = 0;

	testPointsAux[2].x = 0;
	testPointsAux[2].y = offset;
	testPointsAux[2].z = 0;


	//Vector3(this->testPointsAux[0]).y = -offset;
	//Vector3(this->testPointsAux[2]).y = offset;
	Vector3 _rayOrigin;
	_rayOrigin.copy(&hitInfo->position);
	_rayOrigin.add(&hitInfo->normal);

	const Vector3 *pos = &tank->state.pos;

	for (int i = 0; i < 3; i++)
	{
		Vector3 v;
		tank->baseMatrix.transformVector(&testPointsAux[i], &v);
		v.add(pos);
		Vector3 _vector;
		_vector.diff(&v, &hitInfo->position);
		RayIntersection _rayInteresction;
		if (!collisionDetector->intersectRayWithStatic(&_rayOrigin, &_vector, CollisionGroup::STATIC, 1, nullptr, &_rayInteresction))
		{
			return true;
		}
		i++;
	}

	return false;
}

void ThunderModel::applyMainImpactForce(Tank *target, const Vector3 *globalPos, const Vector3 *gunDir,
										double commonImpact, double thunderImpact, double weakeningCoeff)
{
	Vector3 _vector;
	_vector.copy(gunDir);
	_vector.scale(commonImpact * weakeningCoeff);
	target->addWorldForce(globalPos, &_vector);
	_vector.diff(&target->state.pos, globalPos);
	_vector.normalize();
	_vector.scale(thunderImpact * weakeningCoeff);
	target->addForce(&_vector);
}

