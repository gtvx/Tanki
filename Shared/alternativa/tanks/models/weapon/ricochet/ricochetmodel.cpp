#include "ricochetmodel.h"
#include "service.h"
#include "battlecontroller.h"
#include <QHash>
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "ricochettargetsystem.h"
#include "alternativa/tanks/models/weapon/common/IWeaponCommonModel.h"
#include "RicochetData.h"
#include "ricochetshot.h"
#include "flash/utils/gettimer.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/sfx/shoot/ricochet/ricochetsfxmodel.h"
#include "alternativa/tanks/models/sfx/shoot/ricochet/IRicochetSFXModel.h"
#include "alternativa/object/clientobjectweaponricochet.h"
#include "alternativa/object/clientobjectuser.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "randomgenerator.h"
#include "mymath.h"
#include "alternativa/tanks/models/sfx/spriteshoteffect.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/sfx/shoot/ricochet/RicochetSFXData.h"
#include "alternativa/tanks/models/weapon/shared/commontargetevaluator.h"
#include "networking/network.h"
#include "double_to_string.h"


static const double EXPLOSION_SIZE = 266;
static const double SHOT_FLASH_SIZE = 150;
static const int SHOT_FLASH_DURATION = 100;
static const int SHOT_FLASH_FADE = 100;


static void Tank_addWorldForceAtLocalPoint(Tank *tank, const Vector3 *localPos, const Vector3 *worldForce)
{
	Vector3 r;
	tank->baseMatrix.transformVector(localPos, &r);
	r.cross(worldForce);
	tank->forceAccum.add(worldForce);
	tank->torqueAccum.add(&r);
}


RicochetModel::RicochetModel()
{
	weaponCommonModel = nullptr;
	battlefield = nullptr;
	tankModel = nullptr;
	ricochetSfxModel = nullptr;
	localTankData = nullptr;
	localRicochetData = nullptr;
	localRicochetSFXData = nullptr;
	localShotData = nullptr;
	localWeaponCommonData = nullptr;
	triggerPressed = false;
	readyTime = 0;
	currentEnergy = 0;
	targetSystem = nullptr;
	objectPoolService = nullptr;
}

RicochetModel::~RicochetModel()
{

}



void RicochetModel::hit(ClientObjectUser *clientObject,
						const QString &targetId,
						const Vector3 *hitPoint,
						int hitDirectionX,
						int hitDirectionY,
						int hitDirectionZ,
						double weakeningCoeff)
{
	TankData *targetData = this->getTankDataSafe(targetId);
	TankData *td = clientObject->tankData;
	if (targetData == nullptr || td == nullptr)
		return;

	WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(td->turret);
	Vector3 _localPosition;
	Vector3 _hitDirection;
	_localPosition.reset(hitPoint->x, hitPoint->y, hitPoint->z);
	_hitDirection.reset(hitDirectionX / 32767., hitDirectionY / 32767., hitDirectionZ / 32767.);
	_hitDirection.normalize();
	this->battlefield->tankHit(targetData, &_hitDirection, weakeningCoeff * commonData->impactCoeff);
	_hitDirection.scale(weakeningCoeff * commonData->impactForce);
	Tank_addWorldForceAtLocalPoint(targetData->tank, &_localPosition, &_hitDirection);
}

void RicochetModel::fire(ClientObjectUser *clientObject, const QString &shooterId, int shotDirectionX, int shotDirectionY, int shotDirectionZ)
{
	(void)clientObject;

	TankData *shooterData = this->getTankDataSafe(shooterId);
	if ((((shooterData == nullptr) || (shooterData->tank == nullptr)) || (shooterData->tank->skin == nullptr)))
	{
		return;
	}

	if (this->tankModel->localUserData != nullptr)
	{
		if (shooterData == this->tankModel->localUserData)
		{
			return;
		}
	}

	RicochetData *ricochetData = ClientObjectWeaponRicochet::data(shooterData->turret);
	WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(shooterData->turret);

#ifdef GRAPHICS
	RicochetSFXData *sfxData = this->ricochetSfxModel->getSfxData(shooterData->turret);
#else
	RicochetSFXData *sfxData = nullptr;
#endif

	const Vector3 *muzzlePosLocal = commonData->getMuzzle(0);

	Vector3 _muzzlePosGlobal;
	Vector3 _barrelOrigin;
	Vector3 _xAxis;
	Vector3 _gunDirGlobal;

	WeaponUtils::calculateGunParams(shooterData->tank->skin->turretMesh,
									muzzlePosLocal,
									&_muzzlePosGlobal,
									&_barrelOrigin,
									&_xAxis,
									&_gunDirGlobal);

	Vector3 _shotDirection;
	_shotDirection.reset(shotDirectionX / 32767., shotDirectionY / 32767., shotDirectionZ / 32767.);
	_shotDirection.normalize();

	this->createShotEffects(shooterData,
							ricochetData,
							sfxData,
							commonData,
							muzzlePosLocal,
							&_muzzlePosGlobal,
							&_shotDirection,
							&_gunDirGlobal);
}

void RicochetModel::hitSelf(ClientObjectWeapon *clientObject, const QString &shooterId, int relativeHitPosition)
{
	TankData *shooterData = this->getTankDataSafe(shooterId);
	if (shooterData == nullptr)
		return;

	WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(clientObject);
	RicochetSFXData *sfxData = this->ricochetSfxModel->getSfxData(clientObject);
	const Vector3 *muzzlePosLocal = commonData->getMuzzle(0);

	Vector3 _muzzlePosGlobal;
	Vector3 _barrelOrigin;
	Vector3 _xAxis;
	Vector3 _gunDirGlobal;

	WeaponUtils::calculateGunParams(shooterData->tank->skin->turretMesh,
									muzzlePosLocal,
									&_muzzlePosGlobal,
									&_barrelOrigin,
									&_xAxis,
									&_gunDirGlobal);

	this->createSelfHitEffects(shooterData, sfxData, commonData, relativeHitPosition, &_barrelOrigin, &_gunDirGlobal);
}

void RicochetModel::initObject(ClientObjectWeapon *clientObject,
							   int energyCapacity,
							   int energyPerShot,
							   double energyRechargeSpeed,
							   double shotDistance,
							   double shotRadius,
							   double shotSpeed)
{
	std::shared_ptr<RicochetData> weaponData = std::make_shared<RicochetData>((100 * shotRadius),
																			  (100 * shotSpeed),
																			  energyCapacity,
																			  energyPerShot,
																			  energyRechargeSpeed,
																			  (100 * shotDistance));
	ClientObjectWeaponRicochet::setData(clientObject, weaponData);

	WeaponsManager::getRicochetSFXModel(clientObject);
	if (this->battlefield == nullptr)
	{

		this->battlefield = Service::getIBattleField();
		this->tankModel = Service::getTankModel();
		this->weaponCommonModel = Service::getIWeaponCommonModel();
		this->objectPoolService = this->battlefield->getObjectPoolService();
	}
	this->objectLoaded(clientObject);
}

void RicochetModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	if (this->ricochetSfxModel == nullptr)
	{
		this->ricochetSfxModel = WeaponsManager::getRicochetSFXModel(clientObject);
	}
}

void RicochetModel::objectUnloaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
}

void RicochetModel::stopEffects(TankData *ownerTankData)
{
	(void)ownerTankData;
}

void RicochetModel::reset()
{
	this->readyTime = 0;
	this->currentEnergy = this->localRicochetData->energyCapacity;
}


void RicochetModel::setLocalUser(TankData *localUserData)
{
	ClientObjectWeapon *turretObject = localUserData->turret;
	this->localTankData = localUserData;
	this->localRicochetData = ClientObjectWeaponRicochet::data(turretObject);
	this->localRicochetSFXData = ClientObjectWeaponRicochet::SWFData(turretObject);
	this->localShotData = WeaponsManager::shotData(turretObject->getId());
	this->localWeaponCommonData = this->weaponCommonModel->getCommonData(localUserData->turret);
	IBattleField *battleField = Service::getIBattleField();
	TanksCollisionDetector *tanksCollisionDetector = battleField->getBattlefieldData()->collisionDetector;

	CommonTargetEvaluator *commonTargetEvaluator = CommonTargetEvaluator::create(this->localTankData,
																				 this->localShotData,
																				 this->battlefield,
																				 true,
																				 nullptr);

	this->targetSystem = new RicochetTargetSystem(this->localShotData->autoAimingAngleUp,
												  this->localShotData->numRaysUp,
												  this->localShotData->autoAimingAngleDown,
												  this->localShotData->numRaysDown,
												  this->localRicochetData->shotDistance,
												  tanksCollisionDetector,
												  commonTargetEvaluator);

}

void RicochetModel::clearLocalUser()
{
	this->localTankData = nullptr;
	this->localRicochetData = nullptr;
	this->localShotData = nullptr;
	this->localWeaponCommonData = nullptr;
	this->targetSystem = nullptr;
}

void RicochetModel::activateWeapon(int time)
{
	(void)time;
	this->triggerPressed = true;
}

void RicochetModel::deactivateWeapon(int time, bool sendServerCommand)
{
	(void)time;
	(void)sendServerCommand;
	this->triggerPressed = false;
}


double RicochetModel::update(int time, int deltaTime)
{
	int energyCapacity = this->localRicochetData->energyCapacity;
	if (this->currentEnergy < energyCapacity)
	{
		this->currentEnergy = (this->currentEnergy + ((this->localRicochetData->energyRechargeSpeed * deltaTime) * 0.001));
		if (this->currentEnergy > energyCapacity)
		{
			this->currentEnergy = energyCapacity;
		}
	}
	if ((((this->triggerPressed) && (time >= this->readyTime)) && (this->currentEnergy >= this->localRicochetData->energyPerShot)))
	{
		this->doFire(time, this->localWeaponCommonData);
	}
	return this->currentEnergy / this->localRicochetData->energyCapacity;
}

void RicochetModel::shotHit(RicochetShot *shot, const Vector3 *hitPoint, const Vector3 *hitDir, Body *targetBody)
{
	this->createExplosion(hitPoint, shot->sfxData);
	if (shot->shooterData == this->localTankData)
	{
		Tank *targetTank = (Tank*)targetBody;
		TankData *targetTankData = targetTank->tankData;

		Vector3 hitPos;
		hitPos.copy(hitPoint);
		hitPos.subtract(&targetTank->state.pos);
		hitPos.transformBy3Tr(&targetTank->baseMatrix);
		const Vector3 *targetPos = &targetTank->state.pos;

		this->hitCommand(targetTankData->user->getId(),
						 shot->id,
						 targetTankData->incarnation,
						 &hitPos,
						 targetPos,
						 hitDir->x * 32767,
						 hitDir->y * 32767,
						 hitDir->z * 32767,
						 0.01 * shot->totalDistance);
	}
}

void RicochetModel::hitCommand(const QString &victimId,
							   int shotId,
							   int inc,
							   const Vector3 *hitPos3d,
							   const Vector3 *targetPos3d,
							   double x,
							   double y,
							   double z,
							   double distance)
{
	(void)inc;
	(void)targetPos3d;

	//battle;fire;{"y":-29186.624347126995,"shotId":2,"z":4696.118932273324,"hitPos3d":{"y":133.500303336598,"z":-87.1252311265589,"x":272.52005441146224},"reloadTime":600,"victimId":"","distance":30.36,"x":-14133.779225445283}


	QString str;
	str.append("battle;fire;{\"y\":");
	str.append(double_to_string(y));
	str.append(",\"shotId\":");
	str.append(QString::number(shotId));
	str.append(",\"z\":");
	str.append(double_to_string(z));
	str.append(",\"hitPos3d\":{\"y\":");
	str.append(double_to_string(hitPos3d->y));
	str.append(",\"z\":");
	str.append(double_to_string(hitPos3d->z));
	str.append(",\"x\":");
	str.append(double_to_string(hitPos3d->x));
	str.append("},\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append(",\"victimId\":\"");
	str.append(victimId);
	str.append("\",\"distance\":");
	str.append(double_to_string(distance));
	str.append(",\"x\":");
	str.append(double_to_string(x));
	str.append("}");

	Service::getNetworker()->send(str);


#if 0
	var js:Object = new Object();
	js.victimId = victimId;
	js.shotId = shotId;
	js.distance = distance;
	js.hitPos3d = hitPos3d;
	js.x = x;
	js.y = y;
	js.z = z;
	js.reloadTime = this->localShotData.reloadMsec.value;
	Network(Main.osgi.getService(INetworker)).send(("battle;fire;" + JSON.stringify(js)));
#endif
}


void RicochetModel::createExplosion(const Vector3 *position, RicochetSFXData *sfxData)
{
#ifdef GRAPHICS
	AnimatedSpriteEffectNew *explosionEffect = objectPoolService->getAnimatedSpriteEffectNew();
	StaticObject3DPositionProvider *posProvider = objectPoolService->getStaticObject3DPositionProvider();
	posProvider->init(position, 50);
	TextureAnimation *animaton = sfxData->dataExplosion;
	explosionEffect->init(EXPLOSION_SIZE, EXPLOSION_SIZE, animaton, ((RandomGenerator::random() * MyMath::PI) * 2), posProvider, 0.5, 0.5, nullptr, 0);
	this->battlefield->addGraphicEffect(explosionEffect);
	//this->addSoundEffect(sfxData->explosionSound, position, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 0.8);
#else
	(void)position;
	(void)sfxData;
#endif
}

void RicochetModel::addSoundEffect(Sound *sound,
								   const Vector3 *position,
								   double nearRadius,
								   double farRadius,
								   double farDelimiter,
								   double soundVolume)
{
	(void)sound;
	(void)position;
	(void)nearRadius;
	(void)farRadius;
	(void)farDelimiter;
	(void)soundVolume;
#if 0
	var sound3d:Sound3D;
	var soundEffect:Sound3DEffect;
	if (sound != nullptr)
	{
		sound3d = Sound3D.create(sound, nearRadius, farRadius, farDelimiter, soundVolume);
		soundEffect = Sound3DEffect.create(objectPoolService.objectPool, nullptr, position, sound3d);
		this->battlefield.addSound3DEffect(soundEffect);
	}
#endif
}




void RicochetModel::doFire(int time, WeaponCommonData *commonData)
{
	this->readyTime = time + this->localShotData->reloadMsec;
	this->currentEnergy = this->currentEnergy - this->localRicochetData->energyPerShot;
	const Vector3 *muzzlePosLocal = commonData->getMuzzle(0);

	Vector3 _muzzlePosGlobal;
	Vector3 _barrelOrigin;
	Vector3 _xAxis;
	Vector3 _gunDirGlobal;

	WeaponUtils::calculateGunParams(this->localTankData->tank->skin->turretMesh,
									muzzlePosLocal,
									&_muzzlePosGlobal,
									&_barrelOrigin,
									&_xAxis,
									&_gunDirGlobal);

	RayIntersection rayHit;

	if (this->battlefield->getBattlefieldData()->collisionDetector->intersectRayWithStatic(&_barrelOrigin,
																						   &_gunDirGlobal,
																						   CollisionGroup::STATIC,
																						   muzzlePosLocal->y,
																						   nullptr,
																						   &rayHit))
	{
		int relativeHitPosition = int((((rayHit.t / muzzlePosLocal->y) * 0xFF) - 128));
		const Vector3 *pos = &this->localTankData->tank->state.pos;
		Vector3 *pos3d = new Vector3(pos->x, pos->y, pos->z);

		this->createSelfHitEffects(this->localTankData,
								   this->localRicochetSFXData,
								   this->localWeaponCommonData,
								   relativeHitPosition,
								   &_barrelOrigin,
								   &_gunDirGlobal);

		this->hitSelfCommand(this->localTankData->incarnation, relativeHitPosition, pos3d);
	}
	else
	{
		Vector3 v;
		v.copy(&_barrelOrigin);
		v.remove(&this->localTankData->tank->state.pos);
		v.normalize();

		if (this->battlefield->getBattlefieldData()->collisionDetector->intersectRayWithStatic(&this->localTankData->tank->state.pos,
																							   &v,
																							   CollisionGroup::STATIC,
																							   muzzlePosLocal->y,
																							   nullptr,
																							   &rayHit))
		{
			int relativeHitPosition = int((((rayHit.t / muzzlePosLocal->y) * 0xFF) - 128));
			const Vector3 *pos = &this->localTankData->tank->state.pos;
			const Vector3 *pos3d = new Vector3(pos->x, pos->y, pos->z);

			this->createSelfHitEffects(this->localTankData,
									   this->localRicochetSFXData,
									   this->localWeaponCommonData,
									   relativeHitPosition,
									   &_barrelOrigin,
									   &_gunDirGlobal);

			this->hitSelfCommand(this->localTankData->incarnation, relativeHitPosition, pos3d);
		}
		else
		{
			Vector3 _shotDirection;
			if ((!(this->targetSystem->getShotDirection(&_muzzlePosGlobal,
														&_gunDirGlobal,
														&_xAxis,
														this->localTankData->tank,
														&_shotDirection))))
			{
				_shotDirection.copy(&_gunDirGlobal);
			}

			RicochetShot *ricochetShot = this->createShotEffects(this->localTankData,
																 this->localRicochetData,
																 this->localRicochetSFXData,
																 commonData,
																 muzzlePosLocal,
																 &_muzzlePosGlobal,
																 &_shotDirection,
																 &_gunDirGlobal);

			this->fireCommand(ricochetShot->id, _shotDirection.x * 32767, _shotDirection.y * 32767, _shotDirection.z * 32767);
		}
	}
}

void RicochetModel::hitSelfCommand(int inc, int relativeHitPos, const Vector3 *pos3d)
{
	(void)inc;
	(void)relativeHitPos;
	(void)pos3d;

	QString str;
	str.append("battle;fire;{\"self_hit\":true,\"reloadTime\":");
	str.append(QString::number(this->localShotData->reloadMsec));
	str.append("}");

	Service::getNetworker()->send(str);



#if 0
	var js:Object = new Object();
	js.self_hit = true;
	js.reloadTime = this->localShotData.reloadMsec.value;
	Network(Main.osgi.getService(INetworker)).send(("battle;fire;" + JSON.stringify(js)));
#endif
}

void RicochetModel::fireCommand(int shotid, double x, double y, double z)
{
	//battle;start_fire;{"shotid":2,"y":-29186.624347126995,"z":4696.118932273324,"x":-14133.779225445283}

	QString str;
	str.append("battle;start_fire;{\"shotid\":");
	str.append(QString::number(shotid));
	str.append(",\"y\":");
	str.append(double_to_string(y));
	str.append(",\"z\":");
	str.append(double_to_string(z));
	str.append(",\"x\":");
	str.append(double_to_string(x));
	str.append("}");
	Service::getNetworker()->send(str);

#if 0
	var json:Object = new Object();
	json.shotid = shotid;
	json.x = x;
	json.y = y;
	json.z = z;
	Network(Main.osgi.getService(INetworker)).send(("battle;start_fire;" + JSON.stringify(json)));
#endif
}




RicochetShot* RicochetModel::createShotEffects(TankData *shooterData,
											   RicochetData *ricochetData,
											   RicochetSFXData *ricochetSFXData,
											   WeaponCommonData *weaponCommonData,
											   const Vector3 *localMuzzlePosition,
											   const Vector3 *globalMuzzlePosition,
											   const Vector3 *shotDirection,
											   const Vector3 *gunDirection)
{


	RicochetShot *ricochetShot = objectPoolService->getRicochetShot();

	ricochetShot->init(globalMuzzlePosition,
					   shotDirection,
					   shooterData,
					   ricochetData,
					   ricochetSFXData,
					   this->battlefield->getBattlefieldData()->collisionDetector,
					   this,
					   this->battlefield);

	this->battlefield->addGraphicEffect(ricochetShot);
#ifdef GRAPHICS
	SpriteShotEffect *shotFlashEffect = objectPoolService->getSpriteShotEffect();

	shotFlashEffect->init(ricochetSFXData->shotFlashMaterial,
						  localMuzzlePosition,
						  shooterData->tank->skin->turretMesh,
						  20,
						  SHOT_FLASH_SIZE,
						  SHOT_FLASH_DURATION,
						  SHOT_FLASH_FADE,
						  ricochetSFXData->colorTransform);

	this->battlefield->addGraphicEffect(shotFlashEffect);
#else
	(void)localMuzzlePosition;
#endif
	/*
	if (ricochetSFXData->shotSound != nullptr)
	{
		Sound3D sound = Sound3D.create(ricochetSFXData.shotSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 0.8);
		Sound3DEffect soundEffect = Sound3DEffect.create(objectPoolService.objectPool, nullptr, globalMuzzlePosition, sound);
		this->battlefield.addSound3DEffect(soundEffect);
	}
	*/
	shooterData->tank->addWorldForceScaled(globalMuzzlePosition, gunDirection, -weaponCommonData->kickback);
	return ricochetShot;
}

void RicochetModel::createSelfHitEffects(TankData *shooterData,
										 RicochetSFXData *ricochetSFXData,
										 WeaponCommonData *weaponCommonData,
										 int relativeHitPos,
										 const Vector3 *globalBarrelOrigin,
										 const Vector3 *gunDirection)
{
	double offsetAlongBarrel = (((relativeHitPos + 128) / 0xFF) * weaponCommonData->getMuzzle(0)->y);
	Vector3 hitPos;
	hitPos.copy(globalBarrelOrigin);
	hitPos.addScaled(offsetAlongBarrel, gunDirection);
	/*
	if (ricochetSFXData.shotSound != nullptr)
	{
		Sound3D sound = Sound3D.create(ricochetSFXData.shotSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 0.8);
		Sound3DEffect soundEffect = Sound3DEffect.create(objectPoolService.objectPool, nullptr, hitPos, sound);
		this->battlefield.addSound3DEffect(soundEffect);
	}
	*/
	this->createExplosion(&hitPos, ricochetSFXData);
	shooterData->tank->addWorldForceScaled(globalBarrelOrigin, gunDirection, -weaponCommonData->impactForce);

}



TankData *RicochetModel::getTankDataSafe(const QString &objectId)
{
	ClientObjectUser *object = Service::getBattleController()->tanks->value(objectId);
	if (object == nullptr)
	{
		return (nullptr);
	}
	TankData *tankData = this->tankModel->getTankData(object);
	if (((tankData == nullptr) || (tankData->tank == nullptr)))
	{
		return nullptr;
	}
	return tankData;
}
