#include "thundersfxmodel.h"
#include "alternativa/object/clientobjectweaponthunder.h"
#include "resource/resourceutil.h"
#include "alternativa/math/matrix4.h"
#include "thundersfxdata.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "flash/display/bitmapdata.h"
#include "thundershoteffect.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/sfx/effectspair.h"
#include "randomgenerator.h"
#include "mymath.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/tanks/sfx/animatedspriteeffect.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include <QDebug>


static const double MIPMAP_RESOLUTION = 2;
//private static var materialRegistry:IMaterialRegistry;
//private static var objectPoolService:IObjectPoolService;
//private static var turretMatrix:Matrix4 = new Matrix4();
//private static var muzzlePosition:Vector3 = new Vector3();
//private static var explosionSize:ConsoleVarFloat = new ConsoleVarFloat("thunder_explosion_size", 750, 1, 2000);

static const double explosionSize = 750;


static ThunderSFXData* getSfxData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponThunder::SWFData(clientObject);
}


ThunderSFXModel::ThunderSFXModel()
{
	//_interfaces.push(IModel, IThunderSFXModel);
	//materialRegistry = IMaterialRegistry(Main.osgi.getService(IMaterialRegistry));
	//objectPoolService = IObjectPoolService(Main.osgi.getService(IObjectPoolService));
}

ThunderSFXModel::~ThunderSFXModel()
{

}


void ThunderSFXModel::initObject(ClientObjectWeapon *clientObject,
								 const QString &explosionResourceId,
								 const QString &explosionSoundResourceId,
								 const QString &shotResourceId,
								 const QString &shotSoundResourceId)
{
	(void)explosionSoundResourceId;
	(void)shotSoundResourceId;

	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	std::shared_ptr<ThunderSFXData> data = std::make_shared<ThunderSFXData>();
	if (ResourceUtil::getResourceImage(shotResourceId) == nullptr)
	{
		return;
	}

	data->shotMaterial = materialRegistry->textureMaterialRegistry()->getMaterial(
				MaterialType::EFFECT,
				ResourceUtil::getResourceImage(shotResourceId)->bitmapData->data,
				MIPMAP_RESOLUTION, false);

	std::shared_ptr<BitmapData> explosionTexture = ResourceUtil::getResourceImage(explosionResourceId)->bitmapData->data;

	data->explosionMaterials = materialRegistry->materialSequenceRegistry()->getSequence(
				MaterialType::EFFECT,
				explosionTexture.get(),
				explosionTexture->height(), 3)->materials;

	//data->shotSound = ResourceUtil.getResource(ResourceType.SOUND, shotSoundResourceId).sound;
	//data->explosionSound = ResourceUtil.getResource(ResourceType.SOUND, explosionSoundResourceId).sound;
	ClientObjectWeaponThunder::setSWFData(clientObject, data);
}

void ThunderSFXModel::createShotEffects(ClientObjectWeapon *clientObject, const Vector3 *muzzleLocalPos, Object3D *turret, EffectsPair *effectsPair)
{
	ThunderSFXData *data = getSfxData(clientObject);

	ThunderShotEffect *graphicEffect = Service::getIBattleField()->getObjectPoolService()->getThunderShotEffect();
	graphicEffect->init(turret, muzzleLocalPos, data->shotMaterial->ptr.lock());
	//var sound:Sound3D = Sound3D.create(data.shotSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 0.7);
	//Matrix4 turretMatrix;
	//turretMatrix.setMatrix(turret.x, turret.y, turret.z, turret.rotationX, turret.rotationY, turret.rotationZ);
	//turretMatrix.transformVector(muzzleLocalPos, muzzlePosition);
	//var soundEffect:Sound3DEffect = Sound3DEffect.create(objectPoolService.objectPool, null, muzzlePosition, sound);
	effectsPair->graphicEffect = graphicEffect;
	effectsPair->soundEffect = nullptr;
}

void ThunderSFXModel::createExplosionEffects(ClientObjectWeapon *clientObject, const Vector3 *position, EffectsPair *effectsPair)
{
	ThunderSFXData *data = getSfxData(clientObject);
	AnimatedSpriteEffect *graphicEffect = Service::getIBattleField()->getObjectPoolService()->getAnimatedSpriteEffect();

	std::shared_ptr<ColorTransform>  c;
	graphicEffect->init(explosionSize, //1
						explosionSize, //2
						data->explosionMaterials, //3
						position, //4
						(RandomGenerator::random() * 2) * MyMath::PI, //5
						150, //6
						29, //7
						false);

	//Sound3D *sound = Sound3D.create(data.explosionSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, 0.7);
	//Sound3DEffect *soundEffect = Sound3DEffect.create(objectPoolService.objectPool, null, position, sound);

	effectsPair->graphicEffect = graphicEffect;
	effectsPair->soundEffect = nullptr;
}
