#include "weaponsmanager.h"
#include "alternativa/tanks/models/weapon/common/weaponcommonmodel.h"
#include "alternativa/object/clientobjectweapon.h"
#include "alternativa/object/clientobjectweaponsmoky.h"
#include "alternativa/object/clientobjectweaponflamethrower.h"
#include "alternativa/object/clientobjectweaponthunder.h"
#include "alternativa/object/clientobjectweapontwins.h"
#include "alternativa/object/clientobjectweaponricochet.h"
#include "alternativa/object/clientobjectweaponrailgun.h"
#include "alternativa/object/clientobjectweaponfrezee.h"
#include "alternativa/object/clientobjectweaponisida.h"
#include "alternativa/object/clientobjectweaponshaft.h"
#include "alternativa/object/clientobjectweaponterminator.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "alternativa/tanks/models/sfx/shoot/gun/smokysfxmodel.h"
#include "service.h"
#include "_global.h"
#include "alternativa/tanks/models/sfx/flame/flamethrowersfxmodel.h"
#include "alternativa/tanks/models/sfx/shoot/plasma/plasmasfxmodel.h"
#include "alternativa/tanks/models/sfx/shoot/thunder/thundersfxmodel.h"
#include "alternativa/tanks/models/sfx/healing/healinggunsfxmodel.h"
#include "alternativa/tanks/models/sfx/shoot/railgun/railgunsfxmodel.h"
#include "alternativa/tanks/models/weapon/shaft/sfx/shaftsfxmodel.h"
#include "alternativa/tanks/models/weapon/freeze/sfx/freezesfxmodel.h"
#include "alternativa/tanks/models/sfx/shoot/ricochet/ricochetsfxmodel.h"
#include "alternativa/tanks/models/sfx/shoot/railgun/RailgunShootSFXData.h"
#include <QHash>
#include <QMap>
#include <QVariant>
#include <QDebug>


static QHash<QString, ShotData*> shotDatas;
static QHash<QString, ClientObjectWeapon*> turretObjects;
static QMap<QString, QMap<QString, QVariant>> specialEntity;
static QHash<QString, SmokySFXModel*> smokySFXModels;
static QHash<QString, FlamethrowerSFXModel*> flamethrowerSFXModels;
static QHash<QString, PlasmaSFXModel*> twinsSFXModels;
static QHash<QString, ShaftSFXModel*> shaftSFXModel;
static QHash<QString, RailgunSFXModel*> railgunSFXModels;
static QHash<QString, RailgunSFXModel*> terminatorSFXModels;
static ThunderSFXModel *thunderSFXModels;
static HealingGunSFXModel *isidaSFXModel;
static FreezeSFXModel *frezeeSFXModels;
static RicochetSFXModel *ricochetSFXModels;


static ClientObjectWeapon* initObject(const QString &id)
{
	QString name = id.split(QChar('_'))[0];

	if (name == "smoky") {
		return new ClientObjectWeaponSmoky(id, nullptr, id, nullptr);
	} else if (name == "thunder") {
		return new ClientObjectWeaponThunder(id, nullptr, id, nullptr);
	} else if (name == "twins") {
		return new ClientObjectWeaponTwins(id, nullptr, id, nullptr);
	} else if (name == "smokyxt") {
		return new ClientObjectWeaponSmoky(id, nullptr, id, nullptr);
	} else if (name == "ricochet") {
		return new ClientObjectWeaponRicochet(id, nullptr, id, nullptr);
	} else if (name == "railgun") {
		return new ClientObjectWeaponRailgun(id, nullptr, id, nullptr);
	} else if (name == "flamethrower") {
		return new ClientObjectWeaponFlamethrower(id, nullptr, id, nullptr);
	} else if (name == "frezee") {
		return new ClientObjectWeaponFrezee(id, nullptr, id, nullptr);
	} else if (name == "isida") {
		return new ClientObjectWeaponIsida(id, nullptr, id, nullptr);
	} else if (name == "shaft") {
		return new ClientObjectWeaponShaft(id, nullptr, id, nullptr);
	} else if (name == "terminator") {
		return new ClientObjectWeaponTerminator(id, nullptr, id, nullptr);
	} else if (name == "frezeeny") {
		return new ClientObjectWeaponFrezee(id, nullptr, id, nullptr);
	} else if (name == "hwthunder") {
		return new ClientObjectWeaponThunder(id, nullptr, id, nullptr);
	} else if (name == "snowman") {
		return new ClientObjectWeapon(id, nullptr, id, nullptr);
	} else if (name == "twinsxt") {
		return new ClientObjectWeapon(id, nullptr, id, nullptr);
	} else if (name == "pumpkingun") {
		return new ClientObjectWeapon(id, nullptr, id, nullptr);
	}

/*
#ifdef L_GT
	if (name == "flamethrowerhw") {
		return new ClientObjectWeapon(id, nullptr, id, nullptr);
	}
	if (name == "bfg") {
		return new ClientObjectWeapon(id, nullptr, id, nullptr);
	}
	if (name == "isidahw") {
		return new ClientObjectWeapon(id, nullptr, id, nullptr);
	}
#endif
*/
	//qDebug() << name;

	//throw 1234134;
	return new ClientObjectWeapon(id, nullptr, id, nullptr);
}



IWeapon* WeaponsManager::getWeapon(ClientObjectUser *owner, ClientObjectWeapon *clientObject, double impactForce, double kickback, double turretRotationAcceleration, double turretRotationSpeed)
{
	(void)owner;
	WeaponCommonModel *model = Service::getWeaponCommonModel();

	if (model == nullptr)
	{
		model = new WeaponCommonModel();
		Service::setWeaponCommonModel(model);
		Service::setIWeaponCommonModel(model);
	}

	if (!model->initObject(clientObject, impactForce, kickback, turretRotationAcceleration, turretRotationSpeed))
		return nullptr;
	model->objectLoaded(clientObject);
	return model;
}


void WeaponsManager::shotDataInsert(const QString &id, ShotData *shotdata)
{
	shotDatas.insert(id, shotdata);
}

ShotData* WeaponsManager::shotData(const QString &id)
{
	return shotDatas.value(id);
}

ClientObjectWeapon* WeaponsManager::getObjectFor(const QString &id)
{
	if (turretObjects.contains(id) == false)
	{
		turretObjects.insert(id, initObject(id));
	}
	return turretObjects.value(id);
}

QMap<QString, QVariant> WeaponsManager::getSpecialEntity(const QString &id)
{
	return ::specialEntity.value(id);
}

void WeaponsManager::setSpecialEntity(const QString &id, const QVariantMap &specialEntity)
{
	::specialEntity.insert(id, specialEntity);
}

void WeaponsManager::clear()
{
	{
		QHashIterator<QString, ClientObjectWeapon*> i(::turretObjects);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}

	{
		QHashIterator<QString, ShotData*> i(::shotDatas);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}



#ifdef GRAPHICS
	{
		QHashIterator<QString, SmokySFXModel*> i(::smokySFXModels);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}

	{
		QHashIterator<QString, FlamethrowerSFXModel*> i(::flamethrowerSFXModels);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}

	{
		QHashIterator<QString, PlasmaSFXModel*> i(::twinsSFXModels);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}

	{
		QHashIterator<QString, ShaftSFXModel*> i(::shaftSFXModel);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}

	{
		QHashIterator<QString, RailgunSFXModel*> i(::railgunSFXModels);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}

	{
		QHashIterator<QString, RailgunSFXModel*> i(::terminatorSFXModels);
		while (i.hasNext()) {
			i.next();
			delete i.value();
		}
	}


#endif

	::specialEntity.clear();
	::turretObjects.clear();
	::shotDatas.clear();
	::smokySFXModels.clear();
	::flamethrowerSFXModels.clear();
	::twinsSFXModels.clear();
	::shaftSFXModel.clear();
	::railgunSFXModels.clear();
	::terminatorSFXModels.clear();

	if (thunderSFXModels != nullptr)
		delete thunderSFXModels;
	thunderSFXModels = nullptr;

	if (isidaSFXModel != nullptr)
		delete isidaSFXModel;
	isidaSFXModel = nullptr;

	if (frezeeSFXModels != nullptr)
		delete frezeeSFXModels;
	frezeeSFXModels = nullptr;

	if (ricochetSFXModels != nullptr)
		delete ricochetSFXModels;
	ricochetSFXModels = nullptr;



	WeaponCommonModel *model = Service::getWeaponCommonModel();
	if (model != nullptr)
	{
		delete model;
		Service::setWeaponCommonModel(nullptr);
		Service::setIWeaponCommonModel(nullptr);
	}
}


#ifdef GRAPHICS
static SmokySFXModel* getSmokySFX(ClientObjectWeaponSmoky *clientObject)
{
	QString id = clientObject->getId();

	SmokySFXModel *object = smokySFXModels.value(id);

	if (object == nullptr)
	{
		object = new SmokySFXModel();
		object->initObject(clientObject,
						   id + "_explosion",
						   id + "_explosion_sound",
						   id + "_shot",
						   id + "_shot_sound");
		smokySFXModels.insert(id, object);
	}

	return object;
}


ICommonShootSFX* WeaponsManager::getCommonShotSFX(ClientObjectWeapon *turret)
{
	QString id = turret->getId().split("_")[0];

	if (id == "smoky")
		return getSmokySFX((ClientObjectWeaponSmoky*)turret);
/*
	if (id == "hammer")
		return getSmokySFX(turret);

	if (id == "smokyxt")
		return getSmokySFX(turret);
*/
	if (id == "twins")
		return getTwinsSFX(turret);
/*
	if (id == "shaft")
		return nullptr;

	if (id == "snowman")
		return getSnowmanSFX(turret);
*/

	return nullptr;
}

FlamethrowerSFXModel* WeaponsManager::getFlamethrowerSFX(ClientObjectWeapon *clientObject)
{
	QString id = clientObject->getId();

	FlamethrowerSFXModel *f = flamethrowerSFXModels.value(id);

	if (f == nullptr)
	{
		ClientObjectWeaponFlamethrower *famethrower = (ClientObjectWeaponFlamethrower*)clientObject;

		f = new FlamethrowerSFXModel();
		f->initObject(famethrower, id + "_fire", "flamethrower_sound");
		f->objectLoaded(famethrower);

		flamethrowerSFXModels.insert(id, f);
	}

	return f;
}


PlasmaSFXModel* WeaponsManager::getTwinsSFX(ClientObjectWeapon *clientObject)
{
	QString id = clientObject->getId();

	PlasmaSFXModel *f = twinsSFXModels.value(id);

	if (f == nullptr)
	{
		f = new PlasmaSFXModel();
		f->initObject(clientObject,
					  "twins_explosionSound",
					  id + "_explosionTexture",
					  id + "_plasmaTexture",
					  "twins_shotSound",
					  id + "_shotTexture");

		f->objectLoaded(clientObject);

		twinsSFXModels.insert(id, f);
	}

	return f;
}



IThunderSFXModel* WeaponsManager::getThunderSFX(ClientObjectWeapon *clientObject)
{
	if (thunderSFXModels == nullptr)
	{
		thunderSFXModels = new ThunderSFXModel();
	}

	QString id = clientObject->getId();
	thunderSFXModels->initObject(clientObject,
								 id + "_explosionResourceId",
								 "thunder_explosionSoundResourceId",
								 id + "_shotResourceId",
								 "thunder_shotSoundResourceId");

	return thunderSFXModels;
}

HealingGunSFXModel* WeaponsManager::getIsidaSFX(ClientObjectWeapon *clientObject)
{
	if (isidaSFXModel == nullptr)
	{
		isidaSFXModel = new HealingGunSFXModel();
		//Main.osgi.registerService(IHealingSFXModelBase, isidaSFXModel);
	}

	QString id = clientObject->getId();

	isidaSFXModel->initObject(clientObject,
							  id + "_damagingRayId",
							  "damagingSoundId",
							  id + "_damagingTargetBallId",
							  id + "_damagingWeaponBallId",
							  id + "_healingRayId",
							  "healingSoundId",
							  id + "_healingTargetBallId",
							  id + "_healingWeaponBallId",
							  "idleSoundId",
							  id + "_idleWeaponBallId");

	return isidaSFXModel;
}


ShaftSFXModel* WeaponsManager::getShaftSFX(ClientObjectWeapon *clientObject)
{
	QString id = clientObject->getId();

	ShaftSFXModel *object = shaftSFXModel.value(id);

	if (object == nullptr)
	{
		object = new ShaftSFXModel();
		shaftSFXModel.insert(id, object);

		object->initObject(clientObject,
						   "shaft_zoomModeSound",
						   "shaft_targetingSound",
						   id + "_explosionTexture",
						   id + "_trail",
						   id + "_shot");
	}

	return object;
}

FreezeSFXModel* WeaponsManager::getFrezeeSFXModel(ClientObjectWeapon *clientObject)
{
	if (frezeeSFXModels == nullptr)
	{
		frezeeSFXModels = new FreezeSFXModel();
	}

	QString id = clientObject->getId();

	frezeeSFXModels->initObject(clientObject, 17,
								id + "_particleTextureResourceId",
								id + "_planeTextureResourceId",
								"frezee_sound");
	return frezeeSFXModels;
}

RailgunSFXModel* WeaponsManager::getRailgunSFX(ClientObjectWeapon *clientObject)
{
	QString id = clientObject->getId();

	RailgunSFXModel *object = railgunSFXModels.value(id);

	if (object == nullptr)
	{
		object = new RailgunSFXModel();
		railgunSFXModels.insert(id, object);

		std::shared_ptr<RailgunShootSFXData> sfxData = std::make_shared<RailgunShootSFXData>();

		object->initObject(sfxData.get(),
						   id + "_" + "chargingPart1",
						   id + "_" + "chargingPart2",
						   id + "_" + "chargingPart3",
						   id + "_" + "shot",
						   "");

		ClientObjectWeaponRailgun::setSWFData(clientObject, sfxData);
	}

	return object;
}

RailgunSFXModel* WeaponsManager::getTerminatorSFX(ClientObjectWeapon *clientObject)
{
	QString id = clientObject->getId();

	RailgunSFXModel *object = terminatorSFXModels.value(id);

	if (object == nullptr)
	{
		object = new RailgunSFXModel();
		terminatorSFXModels.insert(id, object);

		std::shared_ptr<RailgunShootSFXData> sfxData = std::make_shared<RailgunShootSFXData>();

		object->initObject(sfxData.get(),
						   id + "_" + "chargingPart1",
						   id + "_" + "chargingPart2",
						   id + "_" + "chargingPart3",
						   id + "_" + "shot",
						   "");

		ClientObjectWeaponTerminator::setSWFData(clientObject, sfxData);
	}

	return object;
}

RicochetSFXModel* WeaponsManager::getRicochetSFXModel(ClientObjectWeapon *clientObject)
{
	if (ricochetSFXModels == nullptr)
		ricochetSFXModels = new RicochetSFXModel();

	QString id = clientObject->getId();

	ricochetSFXModels->initObject(clientObject,
								  id + "_bumpFlashTextureId",
								  "ricochet_explosionSoundId",
								  id + "_explosionTextureId",
								  "ricochetSoundId",
								  id + "_shotFlashTextureId",
								  "ricochet_shotSoundId",
								  id + "_shotTextureId",
								  id + "_tailTrailTextureId");
	return ricochetSFXModels;
}

#else

ICommonShootSFX* WeaponsManager::getCommonShotSFX(ClientObjectWeapon*)
{
	return nullptr;
}

HealingGunSFXModel* WeaponsManager::getIsidaSFX(ClientObjectWeapon*)
{
	return nullptr;
}

ShaftSFXModel* WeaponsManager::getShaftSFX(ClientObjectWeapon*)
{
	return nullptr;
}

FreezeSFXModel* WeaponsManager::getFrezeeSFXModel(ClientObjectWeapon*)
{
	return nullptr;
}

RailgunSFXModel* WeaponsManager::getRailgunSFX(ClientObjectWeapon*)
{
	return nullptr;
}

RailgunSFXModel* WeaponsManager::getTerminatorSFX(ClientObjectWeapon*)
{
	return nullptr;
}

RicochetSFXModel* WeaponsManager::getRicochetSFXModel(ClientObjectWeapon*)
{
	return nullptr;
}

#endif







