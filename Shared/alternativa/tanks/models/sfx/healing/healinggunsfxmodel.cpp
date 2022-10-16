#include "healinggunsfxmodel.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "HealingGunSFXData.h"
#include "alternativa/object/clientobjectweaponisida.h"
#include "healinggunsfx.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/weapon/common/IWeaponCommonModel.h"
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/engine3d/materialsequenceregistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "flash/display/bitmapdata.h"


static const double MIPMAP_RESOLUTION = 1;


static TextureAnimation* getTextureAnimation(std::shared_ptr<BitmapData> &data, int size, int fps = 30)
{
	TextureAnimation *data_ = new TextureAnimation();
	GraphicsUtils::getTextureAnimation(data_, data, size, size);
	data_->fps = fps;
	return data_;
}


static QVector<std::shared_ptr<Material>>* createShaftMaterials(std::shared_ptr<BitmapData> &source)
{
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();
	int frameWidth = 100;
	return materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
																	 source.get(),
																	 frameWidth,
																	 MIPMAP_RESOLUTION, false, true)->materials;
}

static QVector<std::shared_ptr<Material>>* createSpriteMaterials(std::shared_ptr<BitmapData> &source)
{
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	return materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT, source.get(), source->height(), MIPMAP_RESOLUTION)->materials;
}




HealingGunSFXModel::HealingGunSFXModel()
{

}



void HealingGunSFXModel::initObject(ClientObjectWeapon *clientObject,
									const QString &damagingRayId,
									const QString &damagingSoundId,
									const QString &damagingTargetBallId,
									const QString &damagingWeaponBallId,
									const QString &healingRayId,
									const QString &healingSoundId,
									const QString &healingTargetBallId,
									const QString &healingWeaponBallId,
									const QString &idleSoundId,
									const QString &idleWeaponBallId)
{
	(void)damagingSoundId;
	(void)healingSoundId;
	(void)idleSoundId;

	//var modelService:IModelService;
	//if (this.battlefield == null)
	{
		//modelService = IModelService(Main.osgi.getService(IModelService));
		//this.battlefield = IBattleField(modelService.getModelsByInterface(IBattleField)[0]);
		//this.weaponCommonModel = IWeaponCommonModel(modelService.getModelsByInterface(IWeaponCommonModel)[0]);
	}

	this->battlefield = Service::getIBattleField();
	this->weaponCommonModel = Service::getIWeaponCommonModel();


	std::shared_ptr<HealingGunSFXData> data = std::make_shared<HealingGunSFXData>();
	data->idleSparkMaterials = createSpriteMaterials(ResourceUtil::getResourceImage(idleWeaponBallId)->bitmapData->data);
	//data->idleSound = ResourceUtil.getResource(ResourceType.SOUND, idleSoundId).sound;
	data->healShaftMaterials = createShaftMaterials(ResourceUtil::getResourceImage(healingRayId)->bitmapData->data);
	data->healSparkMaterials = createSpriteMaterials(ResourceUtil::getResourceImage(healingWeaponBallId)->bitmapData->data);
	data->healShaftEndMaterials = createSpriteMaterials(ResourceUtil::getResourceImage(healingTargetBallId)->bitmapData->data);
	//data->healSound = ResourceUtil.getResource(ResourceType.SOUND, damagingSoundId).sound;
	data->damageShaftMaterials = createShaftMaterials(ResourceUtil::getResourceImage(damagingRayId)->bitmapData->data);
	data->damageSparkMaterials = createSpriteMaterials(ResourceUtil::getResourceImage(damagingWeaponBallId)->bitmapData->data);
	data->damageShaftEndMaterials = createSpriteMaterials(ResourceUtil::getResourceImage(damagingTargetBallId)->bitmapData->data);
	//data->damageSound = ResourceUtil.getResource(ResourceType.SOUND, damagingSoundId).sound;
	data->idleSparkData = getTextureAnimation(ResourceUtil::getResourceImage(idleWeaponBallId)->bitmapData->data, 150, 30);
	data->healSparkData = getTextureAnimation(ResourceUtil::getResourceImage(healingWeaponBallId)->bitmapData->data, 150);
	data->healShaftData = getTextureAnimation(ResourceUtil::getResourceImage(healingRayId)->bitmapData->data, 100, 20);
	data->healShaftEndData = getTextureAnimation(ResourceUtil::getResourceImage(healingTargetBallId)->bitmapData->data, 100);
	data->damageShaftData = getTextureAnimation(ResourceUtil::getResourceImage(damagingRayId)->bitmapData->data, 100, 10);
	data->damageSparkData = getTextureAnimation(ResourceUtil::getResourceImage(damagingWeaponBallId)->bitmapData->data, 150);
	data->damageShaftEndData = getTextureAnimation(ResourceUtil::getResourceImage(damagingTargetBallId)->bitmapData->data, 100);
	ClientObjectWeaponIsida::setSWFData(clientObject, data);
}


void HealingGunSFXModel::createOrUpdateEffects(TankData *ownerData, IsisActionType mode, TankData *targetData)
{
	if (targetData != nullptr && !targetData->enabled)
	{
		return;
	}



	ClientObjectWeapon *ownerTurret = ownerData->turret;
	HealingGunSFXData *sfxData = ClientObjectWeaponIsida::SWFData(ownerTurret);
	WeaponCommonData *weaponCommonData = this->weaponCommonModel->getCommonData(ownerTurret);
	Object3D *turret = ownerData->tank->skin->turretMesh;
	const Vector3 *srcPosLocal = weaponCommonData->getMuzzle(0);
	Object3D* target = (targetData == nullptr) ? nullptr : targetData->tank->skin->turretMesh;
	QString turretId = ownerTurret->getId();

	HealingGunSFX *effects;

	QHash<QString, HealingGunSFX*> *turretEffects = this->activeEffects.value(turretId);



	if (turretEffects == nullptr)
	{
		turretEffects = new QHash<QString, HealingGunSFX*>();
		this->activeEffects.insert(turretId, turretEffects);
		effects = nullptr;
	}
	else
	{
		effects = turretEffects->value(ownerData->user->getId());
	}

	Vector3 targetPosLocal;
	targetPosLocal.reset();

	if (effects == nullptr)
	{
		effects = battlefield->getObjectPoolService()->getHealingGunSFX();
		effects->init(mode, sfxData, turret, srcPosLocal, ownerData);
		if (targetData != nullptr)
		{
			effects->setTargetParams(targetData, ownerData, target, &targetPosLocal, mode);
		}
		turretEffects->insert(ownerData->user->getId(), effects);
		effects->addToBattlefield(this->battlefield);
	}
	else
	{
		effects->setMode(mode);
		effects->setTargetParams(targetData, ownerData, target, &targetPosLocal, mode);
	}
}

void HealingGunSFXModel::destroyEffectsByOwner(TankData *ownerData)
{
	QHash<QString, HealingGunSFX*>* turretEffects = this->activeEffects.value(ownerData->turret->getId());
	if (turretEffects == nullptr)
		return;

	QString userId = ownerData->user->getId();
	HealingGunSFX *effects = turretEffects->value(userId);
	if (effects != nullptr)
	{
		if (effects)
		{
			effects->destroy();
		}
		turretEffects->remove(userId);
		//delete effects;
	}
}

void HealingGunSFXModel::destroyEffectsByTarget(TankData *targetData)
{
	QHashIterator<QString, QHash<QString, HealingGunSFX*>*> i(this->activeEffects);
	while (i.hasNext())
	{
		i.next();
		QHash<QString, HealingGunSFX*> *turretEffects = i.value();
		if (turretEffects != nullptr)
		{
			QHash<QString, HealingGunSFX*>::iterator i2 = turretEffects->begin();
			//QHashIterator<QString, HealingGunSFX*> i2(*turretEffects);
			while (i2 != turretEffects->end())
			{
				HealingGunSFX *effects = i2.value();
				if (effects->targetData == targetData)
				{
					effects->destroy();
					i2 = turretEffects->erase(i2);
				}
				else
				{
					++i2;
				}
			}
		}
	}
}
