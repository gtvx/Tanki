#include "freezesfxmodel.h"
#include "alternativa/object/clientobjectweaponfrezee.h"
#include "freezesfxdata.h"
#include "flash/display/bitmapdata.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "freezegraphiceffect.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "battlecontroller.h"
#include "../freezemodel.h"
#include "../freezedata.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/sfx/muzzlepositionprovider.h"
#include "alternativa/tanks/models/sfx/omnistreamlighteffect.h"
#include "alternativa/tanks/models/sfx/collisionobject3dpositionprovider.h"
#include "alternativa/tanks/models/sfx/lightdatamanager.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"



static const double MIPMAP_RESOLUTION = 2;


static FreezeSFXData* getSFXData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponFrezee::SWFData(clientObject);
}


FreezeSFXModel::FreezeSFXModel()
{
	this->bfModel = Service::getBattlefieldModel();
	this->freezeModel = Service::getBattleController()->frezeeModel;
	this->muzzleLightEffect = nullptr;
	this->lightEffect = nullptr;
}

FreezeSFXModel::~FreezeSFXModel()
{

}


void FreezeSFXModel::initObject(ClientObjectWeapon *clientObject,
								double particleSpeed,
								const QString &particleTextureResourceId,
								const QString &planeTextureResourceId,
								const QString &shotSoundResourceId)
{
	(void)shotSoundResourceId;

	std::shared_ptr<FreezeSFXData> sfxData = std::make_shared<FreezeSFXData>();
	std::shared_ptr<BitmapData> particleTexture = ResourceUtil::getResourceImage(particleTextureResourceId)->bitmapData->data;


	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	//MaterialType, BitmapData*, int, double, bool = false, bool = false

	sfxData->particleMaterials = materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
																						   particleTexture.get(),
																						   particleTexture->height(),
																						   MIPMAP_RESOLUTION)->materials;

	std::shared_ptr<BitmapData> planeTexture = ResourceUtil::getResourceImage(planeTextureResourceId)->bitmapData->data;

	sfxData->planeMaterials = materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
																						planeTexture.get(),
																						250,
																						MIPMAP_RESOLUTION)->materials;
	sfxData->particleSpeed = 100 * particleSpeed;
	//sfxData->shotSound = ResourceUtil::getResource(ResourceType.SOUND, shotSoundResourceId).sound;
	ClientObjectWeaponFrezee::setSWFData(clientObject, sfxData);
}



void FreezeSFXModel::createEffects(TankData *tankData, WeaponCommonData *weaponCommonData)
{
	FreezeSFXData *sfxData = getSFXData(tankData->turret);
	if (sfxData == nullptr)
	{
		//message = "SFX data not found";
		//Logger.log(LogLevel.LOG_ERROR, message);
		//throw (new Error(message));
		throw 24524524524;
	}

	FreezeData *freezeData = this->freezeModel->getFreezeData(tankData->turret);
	if (freezeData == nullptr)
	{
		//message = "Freeze data not found";
		//Logger.log(LogLevel.LOG_ERROR, message);
		//throw (new Error(message));
		throw 24524524525;
	}

	ObjectPoolService *pool = bfModel->getObjectPoolService();

	FreezeGraphicEffect *graphicEffect = pool->getFreezeGraphicEffect();

	graphicEffect->init(tankData,
						freezeData->damageAreaRange,
						freezeData->damageAreaConeAngle,
						sfxData->particleSpeed,
						weaponCommonData->getMuzzle(0),
						tankData->tank->skin->turretMesh,
						sfxData,
						this->bfModel->getBattlefieldData()->collisionDetector);

	this->bfModel->addGraphicEffect(graphicEffect);
	this->createLightEffect(weaponCommonData->getMuzzle(0), tankData->tank->skin->turretMesh, tankData->turret);
	this->graphicEffects.insert(tankData, graphicEffect);

	//if (sfxData.shotSound != nullptr)
	//{
	//Sound3D sound3d = Sound3D.create(sfxData.shotSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, SOUND_VOLUME);
	//MobileSound3DEffect soundEffect = MobileSound3DEffect(objectPoolService.objectPool.getObject(MobileSound3DEffect));
	//soundEffect.init(null, sound3d, tankData.tank.skin.turretMesh, 0, 1);
	//this->battlefield.addSound3DEffect(soundEffect);
	//this->soundEffects[tankData] = soundEffect;
	//}
}



void FreezeSFXModel::createLightEffect(const Vector3 *pos, Object3D *turret, ClientObject *turretObject)
{
	ObjectPoolService *pool = bfModel->getObjectPoolService();
	QString id = turretObject->getId();

	if (this->muzzleLightEffect == nullptr)
	{
		this->muzzleLightEffect = pool->getOmniStreamLightEffect();
		MuzzlePositionProvider *m = pool->getMuzzlePositionProvider();
		m->init(turret, pos);
		this->muzzleLightEffect->init(m,
									  LightDataManager::getLightDataMuzzle(id),
									  LightDataManager::getLightDataShot(id));
		this->bfModel->addGraphicEffect(this->muzzleLightEffect);
	}
	if (this->lightEffect == nullptr)
	{
		this->lightEffect = pool->getOmniStreamLightEffect();
		CollisionObject3DPositionProvider *c = pool->getCollisionObject3DPositionProvider();
		c->init(turret, pos, this->bfModel->bfData->collisionDetector, 1500);

		this->lightEffect->init(c,
								LightDataManager::getLightDataMuzzle(id),
								LightDataManager::getLightDataShot(id));
		this->bfModel->addGraphicEffect(this->lightEffect);
	}
}

void FreezeSFXModel::destroyEffects(TankData *tankData)
{
	IGraphicEffect *graphicEffect = this->graphicEffects.value(tankData);
	if (graphicEffect != nullptr)
	{
		this->graphicEffects.remove(tankData);
		graphicEffect->kill();
		if (this->muzzleLightEffect != nullptr)
		{
			this->muzzleLightEffect->stop();
			this->muzzleLightEffect = nullptr;
		}
		if (this->lightEffect != nullptr)
		{
			this->lightEffect->stop();
			this->lightEffect = nullptr;
		}
	}

	//ISound3DEffect *soundEffect = this->soundEffects[tankData];
	//if (soundEffect != nullptr)
	//{
	//delete this->soundEffects[tankData];
	//soundEffect.kill();
	//}
}

void FreezeSFXModel::objectLoaded(ClientObject *object)
{
	(void)object;
#if 0
	IColorTransformModel colorTransformModel = IColorTransformModel(modelService.getModelForObject(object, IColorTransformModel));
	if (colorTransformModel != nullptr)
	{
		QVector<ColorTransformEntry> colorTransforms = colorTransformModel.getModelData(object);
		if (colorTransforms != nullptr)
		{
			FreezeSFXData *sfxData = this->getSFXData(object);
			sfxData.colorTransformPoints = colorTransforms;
		}
	}
#endif
}

void FreezeSFXModel::objectUnloaded(ClientObject *object)
{
	(void)object;
}

