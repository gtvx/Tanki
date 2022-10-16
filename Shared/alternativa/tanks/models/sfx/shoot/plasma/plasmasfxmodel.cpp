#include "plasmasfxmodel.h"
#include "plasmasfxdata.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/tanks/models/weapon/plasma/plasmashot.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/object/clientobjectweapontwins.h"
#include "alternativa/tanks/models/sfx/spriteshoteffect.h"
#include "alternativa/tanks/sfx/effectspair.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "randomgenerator.h"
#include "flash/geom/colortransform.h"
#include "alternativa/tanks/models/sfx/colortransform/colortransformentry.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"


PlasmaSFXModel::PlasmaSFXModel()
{
	//_interfaces.push(IModel, IPlasmaShootSFXModelBase, ICommonShootSFX, IPlasmaSFX, IObjectLoadListener);
	//materialRegistry = IMaterialRegistry(Main.osgi.getService(IMaterialRegistry));
	//objectPoolService = IObjectPoolService(Main.osgi.getService(IObjectPoolService));
	materialRegistry = Service::getIMaterialRegistry();
}

PlasmaSFXModel::~PlasmaSFXModel()
{

}



void PlasmaSFXModel::initObject(ClientObjectWeapon *clientObject, const QString &explosionSoundId, const QString &explosionTextureId,
								const QString &plasmaTextureId, const QString &shotSoundId, const QString &shotTextureId)
{
	(void)explosionSoundId;
	(void)shotSoundId;

	std::shared_ptr<PlasmaSFXData> data = std::make_shared<PlasmaSFXData>();
	std::shared_ptr<BitmapData> flashTexture = ResourceUtil::getResourceImage(shotTextureId)->bitmapData->data;

	data->shotFlashMaterial = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT, flashTexture, 1, false);

	std::shared_ptr<BitmapData> plasmaTexture = ResourceUtil::getResourceImage(plasmaTextureId)->bitmapData->data;

	data->shotMaterials = materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
															  plasmaTexture.get(),
															  plasmaTexture->height(),
															  PlasmaShot::SIZE / plasmaTexture->height())->materials;


	std::shared_ptr<BitmapData> explosionTexture = ResourceUtil::getResourceImage(explosionTextureId)->bitmapData->data;
	data->explosionMaterials = materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
																						 explosionTexture.get(),
																						 explosionTexture->height(), 3)->materials;
	//data->shotSound = ResourceUtil.getResource(ResourceType.SOUND, shotSoundId).sound;
	//data->explosionSound = ResourceUtil::getResource(ResourceType.SOUND, explosionSoundId).sound;

	data->explosionData = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(data->explosionData, explosionTexture, 200, 200);
	data->explosionData->fps = 30;
	data->shotData = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(data->shotData, plasmaTexture, 150, 150);
	data->shotData->fps = 20;
	ClientObjectWeaponTwins::setSWFData(clientObject, data);
}



void PlasmaSFXModel::createShotEffects(ClientObjectWeapon *clientObject, const Vector3 *muzzleLocalPos, Object3D *turret, Camera3D *camera, EffectsPair *effectsPair)
{
	(void)camera;


	PlasmaSFXData *data = this->getPlasmaSFXData(clientObject);
	SpriteShotEffect *shotEffect = Service::getIBattleField()->getObjectPoolService()->getSpriteShotEffect();
	shotEffect->init(data->shotFlashMaterial, muzzleLocalPos, turret, 10, 120, 50, 50, data->colorTransform);
	/*
	if (data.shotSound != nullptr)
	{
		Sound3D *sound = Sound3D.create(data.shotSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 0.8);
		turretPosition.x = turret.x;
		turretPosition.y = turret.y;
		turretPosition.z = turret.z;
		Sound3DEffect *soundEffect = Sound3DEffect.create(objectPoolService.objectPool, null, turretPosition, sound);
	}
	*/
	effectsPair->graphicEffect = shotEffect;
	effectsPair->soundEffect = nullptr;
}


void PlasmaSFXModel::createExplosionEffects(ClientObjectWeapon *clientObject, const Vector3 *pos, Camera3D *camera, double weakeningCoeff, EffectsPair *effectsPair)
{
	(void)camera;
	PlasmaSFXData *data = this->getPlasmaSFXData(clientObject);
	double size = 300 * (1 + weakeningCoeff);
	AnimatedSpriteEffectNew *explosionEffect = Service::getIBattleField()->getObjectPoolService()->getAnimatedSpriteEffectNew();
	TextureAnimation *animation = data->explosionData;
	StaticObject3DPositionProvider *position = Service::getIBattleField()->getObjectPoolService()->getStaticObject3DPositionProvider();
	position->init(pos, 50);
	explosionEffect->init(size, size, animation, 6.28 * RandomGenerator::random(), position);
	effectsPair->graphicEffect = explosionEffect;
	effectsPair->soundEffect = nullptr;
}

PlasmaSFXData* PlasmaSFXModel::getPlasmaSFXData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponTwins::SWFData(clientObject);
}


void PlasmaSFXModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
	/*
	//var colorTransforms:Vector.<ColorTransformEntry>;
	//IModelService modelService = IModelService(Main.osgi.getService(IModelService));
	//IColorTransformModel *colorTransformModel = IColorTransformModel(modelService.getModelForObject(clientObject, IColorTransformModel));
	if (colorTransformModel != nullptr)
	{
		PlasmaSFXData *sfxData = this->getPlasmaSFXData(clientObject);
		//(Vector.<ColorTransformEntry>(clientObject.getParams(ColorTransformModel)));
		clientObject->

		colorTransforms = colorTransformModel->getModelData(clientObject);
		ColorTransformEntry *ctStruct = colorTransforms[0];
		sfxData->colorTransform = std::make_shared<ColorTransform>(ctStruct->redMultiplier,
																   ctStruct->greenMultiplier,
																   ctStruct->blueMultiplier,
																   ctStruct->alphaMultiplier,
																   ctStruct->redOffset,
																   ctStruct->greenOffset,
																   ctStruct->blueOffset,
																   ctStruct->alphaOffset);
	}
	*/
}

void PlasmaSFXModel::objectUnloaded(ClientObject *clientObject)
{
	(void)clientObject;
}
