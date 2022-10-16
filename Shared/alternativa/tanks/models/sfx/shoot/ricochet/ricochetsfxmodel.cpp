#include "ricochetsfxmodel.h"
#include "alternativa/object/clientobjectweaponricochet.h"
#include "RicochetSFXData.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/engine3d/materials/texturematerial.h"


RicochetSFXModel::RicochetSFXModel()
{

}

RicochetSFXModel::~RicochetSFXModel()
{

}

void RicochetSFXModel::initObject(ClientObjectWeapon *clientObject,
								  const QString &bumpFlashTextureId,
								  const QString &explosionSoundId,
								  const QString &explosionTextureId,
								  const QString &ricochetSoundId,
								  const QString &shotFlashTextureId,
								  const QString &shotSoundId,
								  const QString &shotTextureId,
								  const QString &tailTrailTextureId)
{
	(void)explosionSoundId;
	(void)ricochetSoundId;
	(void)shotSoundId;

	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	std::shared_ptr<RicochetSFXData> sfxData = std::make_shared<RicochetSFXData>();

	IMaterialSequenceRegistry *materialSequenceRegistry = materialRegistry->materialSequenceRegistry();
	std::shared_ptr<BitmapData> shotTexture = ResourceUtil::getResourceImage(shotTextureId)->bitmapData->data;
	sfxData->shotMaterials = materialSequenceRegistry->getSquareSequence(MaterialType::EFFECT, shotTexture.get(), 2)->materials;
	std::shared_ptr<BitmapData> explosionTexture = ResourceUtil::getResourceImage(explosionTextureId)->bitmapData->data;
	sfxData->explosionMaterials = materialSequenceRegistry->getSquareSequence(MaterialType::EFFECT, explosionTexture.get(), 1.33)->materials;
	std::shared_ptr<BitmapData> bumpFlashTexture = ResourceUtil::getResourceImage(bumpFlashTextureId)->bitmapData->data;
	sfxData->ricochetFlashMaterials = materialSequenceRegistry->getSquareSequence(MaterialType::EFFECT, bumpFlashTexture.get(), 0.4)->materials;
	ITextureMaterialRegistry *textureMaterialRegistry = materialRegistry->textureMaterialRegistry();
	std::shared_ptr<BitmapData> shotFlashTexture = ResourceUtil::getResourceImage(shotFlashTextureId)->bitmapData->data;
	sfxData->shotFlashMaterial = textureMaterialRegistry->getMaterial(MaterialType::EFFECT, shotFlashTexture, 1);
	std::shared_ptr<BitmapData> tailTrailTexture = ResourceUtil::getResourceImage(tailTrailTextureId)->bitmapData->data;
	sfxData->tailTrailMaterial = textureMaterialRegistry->getMaterial(MaterialType::EFFECT, tailTrailTexture, 2);
	//sfxData->shotSound = ResourceUtil::getResource(ResourceType.SOUND, shotSoundId).sound;
	//sfxData->ricochetSound = ResourceUtil::getResource(ResourceType.SOUND, ricochetSoundId).sound;
	//sfxData->explosionSound = ResourceUtil::getResource(ResourceType.SOUND, explosionSoundId).sound;
	sfxData->dataExplosion = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(sfxData->dataExplosion, explosionTexture, 200, 200);
	sfxData->dataExplosion->fps = 30;
	sfxData->dataFlash = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(sfxData->dataFlash, explosionTexture, 200, 200);
	sfxData->dataFlash->fps = 45;
	sfxData->dataShot = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(sfxData->dataShot, shotTexture, 150, 150);
	sfxData->dataShot->fps = 13;
	ClientObjectWeaponRicochet::setSWFData(clientObject, sfxData);

}

void RicochetSFXModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
	/*
	var colorTransforms:Vector.<ColorTransformEntry>;
	var ctStruct:ColorTransformEntry;
	var modelService:IModelService = IModelService(Main.osgi.getService(IModelService));
	var colorTransformModel:IColorTransformModel = IColorTransformModel(modelService.getModelForObject(object, IColorTransformModel));
	if (colorTransformModel != null)
	{
		RicochetSFXData *sfxData = this.getSfxData(object);
		colorTransforms = colorTransformModel.getModelData(object);
		if (colorTransforms.length > 0)
		{
			ctStruct = colorTransforms[0];
			sfxData->colorTransform = new ColorTransform(ctStruct.redMultiplier, ctStruct.greenMultiplier, ctStruct.blueMultiplier, ctStruct.alphaMultiplier, ctStruct.redOffset, ctStruct.greenOffset, ctStruct.blueOffset, ctStruct.alphaOffset);
		}
	}
	*/
}

void RicochetSFXModel::objectUnloaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
}

RicochetSFXData* RicochetSFXModel::getSfxData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponRicochet::SWFData(clientObject);
}
