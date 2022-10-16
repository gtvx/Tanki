#include "smokysfxmodel.h"
#include "gunshootsfxdata.h"
#include "alternativa/tanks/sfx/planemuzzleflasheffect.h"
#include "alternativa/object/clientobjectweaponsmoky.h"
#include "alternativa/tanks/sfx/effectspair.h"
#include "resource/resourceutil.h"
#include "resource/images/objectbitmapdata.h"
#include "resource/images/imageresource.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "file.h"
#include <QDebug>

/*
private static const SHOT_SOUND_VOLUME:Number = 1;
private static const EXPLOSION_BASE_SIZE:Number = 600;
private static const EXPLOSION_OFFSET_TO_CAMERA:Number = 135;
private static var objectPoolService:IObjectPoolService;
private static var materialRegistry:IMaterialRegistry;
private static var MIPMAP_RESOLUTION:Number = 2;
private static var position:Vector3 = new Vector3();
*/

static const double EXPLOSION_BASE_SIZE = 600;
static const double MIPMAP_RESOLUTION = 2;
static const double EXPLOSION_OFFSET_TO_CAMERA = 135;



static GunShootSFXData* getSfxData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponSmoky::SWFData(clientObject);
}


SmokySFXModel::SmokySFXModel()
{
	//_interfaces.push(IModel, IGunShootSFXModelBase, ICommonShootSFX);
}

SmokySFXModel::~SmokySFXModel()
{

}

void SmokySFXModel::initObject(ClientObjectWeapon *clientObject, const QString &explosionResourceId, const QString &explosionSoundResourceId, const QString &shotResourceId, const QString &shotSoundResourceId)
{
	(void)explosionSoundResourceId;
	(void)shotSoundResourceId;

	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	std::shared_ptr<GunShootSFXData> data = std::make_shared<GunShootSFXData>();

	std::shared_ptr<BitmapData> shotTexture = ResourceUtil::getResourceImage(shotResourceId)->bitmapData->data;

	data->shotMaterial = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT,
																				shotTexture,
																				EXPLOSION_BASE_SIZE / shotTexture->width(),
																				false);

	std::shared_ptr<BitmapData> explosionTexture = ResourceUtil::getResourceImage(explosionResourceId)->bitmapData->data;

	MaterialSequence *sequence = materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
																						   explosionTexture.get(),
																						   explosionTexture->height(),
																						   MIPMAP_RESOLUTION);



	(void)sequence;


	//data->explosionMaterials = sequence->materials;
	//data->explosionMaterials = nullptr;
	data->shotSound = nullptr; //(ResourceUtil.getResource(ResourceType.SOUND, "smoky_shot").sound as Sound);
	data->explosionSound = nullptr; //(ResourceUtil.getResource(ResourceType.SOUND, "smoky_exp").sound as Sound);

	TextureAnimation *animExpl = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(animExpl, explosionTexture, 200, 200);
	animExpl->fps = 30;

	TextureAnimation *animShot = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(animShot, shotTexture, 30, 105);
	animShot->fps = 1;

	data->explosionData = animExpl;
	data->shotData = animShot;

	ClientObjectWeaponSmoky::setSWFData(clientObject, data);
}

void SmokySFXModel::createShotEffects(ClientObjectWeapon *clientObject, const Vector3 *localMuzzlePosition, Object3D *turret, Camera3D *camera, EffectsPair *effectsPair)
{
	(void)turret;
	(void)camera;

	GunShootSFXData *data = ::getSfxData(clientObject);

	//Sound3D *sound = Sound3D.create(data.shotSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, SHOT_SOUND_VOLUME);

	/*
	Vector3 position;
	position.x = turret.x;
	position.y = turret.y;
	position.z = turret.z;
	*/
	//Sound3DEffect *soundEffect = Sound3DEffect.create(objectPoolService.objectPool, null, position, sound);

	PlaneMuzzleFlashEffect *graphicEffect = Service::getIBattleField()->getObjectPoolService()->getPlaneMuzzleFlashEffect();

	graphicEffect->init(localMuzzlePosition, turret, (TextureMaterial*)data->shotMaterial, 100, 60, 210);

	effectsPair->graphicEffect = graphicEffect;
	effectsPair->soundEffect = nullptr;
}

void SmokySFXModel::createExplosionEffects(ClientObjectWeapon *clientObject, const Vector3 *position, Camera3D *camera, double weakeningCoeff, EffectsPair *effectsPair)
{
	(void)camera;

	GunShootSFXData *data = ::getSfxData(clientObject);
	//Sound3D sound = Sound3D.create(data.explosionSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 1);
	//Sound3DEffect soundEffect = Sound3DEffect.create(objectPoolService.objectPool, null, position, sound, 100);

	AnimatedSpriteEffectNew *graphicEffect = Service::getIBattleField()->getObjectPoolService()->getAnimatedSpriteEffectNew();
	StaticObject3DPositionProvider *posProvider = Service::getIBattleField()->getObjectPoolService()->getStaticObject3DPositionProvider();

	posProvider->init(position, EXPLOSION_OFFSET_TO_CAMERA);
	double size = EXPLOSION_BASE_SIZE * weakeningCoeff;

	graphicEffect->init(size, size, data->explosionData, 0, posProvider);

	effectsPair->graphicEffect = graphicEffect;
	effectsPair->soundEffect = nullptr;
}
