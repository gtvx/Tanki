#include "shaftsfxmodel.h"
#include "alternativa/tanks/models/sfx/shoot/shaft/ShaftTrailEffect.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "service.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "ShaftSFXData.h"
#include "alternativa/tanks/models/sfx/shoot/shaft/traileffect1.h"
#include "alternativa/tanks/models/sfx/shoot/shaft/traileffect2.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/object/clientobjectweaponshaft.h"
#include "alternativa/tanks/models/sfx/muzzlepositionprovider.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "alternativa/tanks/engine3d/textureanimation.h"



//private static const ZOOM_MODE_SOUND_VOLUME:Number = 0.7;
//public static const MUZZLE_FLASH_SIZE:Number = 200;
static const double EXPLOSION_WIDTH = 250;
static const double EXPLOSION_OFFSET_TO_CAMERA = 110;
//private static const EXPLOSION_SOUND_VOLUME:Number = 0.8;
//private static const CHARGING_SOUND_FADE_TIME_MS:int = 1000;
//private static const vectorToHitPoint:Vector3 = new Vector3();
static const int TRAIL_DURATION = 1000;
//private static var objectPoolService:IObjectPoolService;


ShaftSFXModel::ShaftSFXModel()
{
	this->battlefieldModel = Service::getBattlefieldModel();
	this->objectPoolService = this->battlefieldModel->getObjectPoolService();

}


void ShaftSFXModel::initObject(ClientObjectWeapon *clientObject,
							   const QString &manualModeEffect,
							   const QString &targetingSound,
							   const QString &explosionId,
							   const QString &trailTexture,
							   const QString &muzzleFlashTexture)
{	
	(void)manualModeEffect;
	(void)targetingSound;

	std::shared_ptr<ShaftSFXData> sfxData = std::make_shared<ShaftSFXData>();
	//sfxData.zoomModeSound = (ResourceUtil.getResource(ResourceType.SOUND, manualModeEffect).sound as Sound);
	//sfxData.targetingSound = (ResourceUtil.getResource(ResourceType.SOUND, targetingSound).sound as Sound);
	//sfxData.shotSound = (ResourceUtil.getResource(ResourceType.SOUND, "shaft_shot").sound as Sound);
	//sfxData.explosionSound = (ResourceUtil.getResource(ResourceType.SOUND, "shaft_explosion_sound").sound as Sound);
	std::shared_ptr<BitmapData> trailBitmapData = ResourceUtil::getResourceImage(trailTexture)->bitmapData->data;
	std::shared_ptr<TextureMaterial> trailMaterial = new_TextureMaterial();
	trailMaterial->init();
	trailMaterial->setTexture(trailBitmapData);
	sfxData->trailMaterial = trailMaterial;
	std::shared_ptr<BitmapData> exlpBitmap = ResourceUtil::getResourceImage(explosionId)->bitmapData->data;
	sfxData->explosionAnimation = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(sfxData->explosionAnimation, exlpBitmap, 200, 200);
	sfxData->explosionAnimation->fps = 30;
	std::shared_ptr<BitmapData> muzzleFlash = ResourceUtil::getResourceImage(muzzleFlashTexture)->bitmapData->data;
	sfxData->muzzleFlashAnimation = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(sfxData->muzzleFlashAnimation, muzzleFlash, 60, 53);
	sfxData->muzzleFlashAnimation->fps = 4;
	ClientObjectWeaponShaft::setSWFData(clientObject, sfxData);
}

MobileSound3DEffect* ShaftSFXModel::createManualModeEffects(ClientObjectWeapon *turretObj, ClientObjectUser *clientObject, Object3D *obj)
{
	(void)turretObj;
	(void)clientObject;
	(void)obj;

	//ShaftSFXData *sfxData = turretObj->shaftSFXModel;
	//ShaftSFXUserData soundData = ((!(clientObject.getParams(ShaftSFXUserData) == null)) ? (clientObject.getParams(ShaftSFXUserData) as ShaftSFXUserData) : new ShaftSFXUserData());
	//soundData.manualModeEffect = MobileSound3DEffect(objectPoolService.objectPool.getObject(MobileSound3DEffect));
	//var sound:Sound3D = Sound3D.create(sfxData.zoomModeSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, ZOOM_MODE_SOUND_VOLUME);
	//soundData.manualModeEffect.init(null, sound, obj, 0, 9999);
	//this->battlefieldModel.addSound3DEffect(soundData.manualModeEffect);
	//clientObject.putParams(ShaftSFXUserData, soundData);
	//return (soundData.manualModeEffect);
	return nullptr;
}

void ShaftSFXModel::fadeChargingEffect(ClientObjectWeapon *turretObj, ClientObjectUser *clientObject)
{
	(void)turretObj;
	(void)clientObject;
	//var sfxData:ShaftSFXData = (turretObj.getParams(ShaftSFXModel) as ShaftSFXData);
	//var soundData:ShaftSFXUserData = ((!(clientObject.getParams(ShaftSFXUserData) == null)) ? (clientObject.getParams(ShaftSFXUserData) as ShaftSFXUserData) : new ShaftSFXUserData());
	//if (soundData.manualModeEffect != null)
	//	soundData.manualModeEffect.fade(CHARGING_SOUND_FADE_TIME_MS);
}

void ShaftSFXModel::createShotSoundEffect(ClientObjectWeapon *turretObj, ClientObjectUser *owner, Vector3 *param1)
{
	(void)turretObj;
	(void)owner;
	(void)param1;
	//var sfxData:ShaftSFXData = (turretObj.getParams(ShaftSFXModel) as ShaftSFXData);
	//var _loc2_:Sound3D = Sound3D.create(sfxData.shotSound, 1000, 5000, 5, this->SHOT_SOUND_VOLUME);
	//var _loc3_:Sound3DEffect = Sound3DEffect.create(objectPoolService.objectPool, owner, param1, _loc2_);
	//this->battlefieldModel.addSound3DEffect(_loc3_);
}



void ShaftSFXModel::createMuzzleFlashEffect(ClientObjectWeapon *turretObj, Vector3 *param1, Object3D *param2)
{
	ShaftSFXData *sfxData = ClientObjectWeaponShaft::SWFData(turretObj);
	MuzzlePositionProvider *pos = objectPoolService->getMuzzlePositionProvider();
	pos->init(param2, param1, 10);
	AnimatedSpriteEffectNew *animSprite = objectPoolService->getAnimatedSpriteEffectNew();
	animSprite->init(70, 63, sfxData->muzzleFlashAnimation, 0, pos);
	this->battlefieldModel->addGraphicEffect(animSprite);
}

void ShaftSFXModel::stopManualSound(ClientObjectUser *clientObject)
{
	(void)clientObject;
	/*
	var soundData:ShaftSFXUserData = ((clientObject.getParams(ShaftSFXUserData) == null) ? null : (clientObject.getParams(ShaftSFXUserData) as ShaftSFXUserData));
	if (soundData == null)
	{
		soundData = new ShaftSFXUserData();
		clientObject.putParams(ShaftSFXUserData, soundData);
	};
	if (soundData.manualModeEffect != null)
	{
		soundData.manualModeEffect.kill();
	}
	*/
}

void ShaftSFXModel::playTargetingSound(ClientObjectWeapon *turretObj, ClientObjectUser *clientObject, bool play)
{
	(void)turretObj;
	(void)clientObject;
	(void)play;

	/*
	var alert:AlertBugWindow;
	var sfxData:ShaftSFXData;
	var soundData:ShaftSFXUserData;
	alert = null;
	try
	{
		sfxData = (turretObj.getParams(ShaftSFXModel) as ShaftSFXData);
		soundData = ((clientObject.getParams(ShaftSFXUserData) == null) ? null : (clientObject.getParams(ShaftSFXUserData) as ShaftSFXUserData));
		if (soundData == null)
		{
			soundData = new ShaftSFXUserData();
			clientObject.putParams(ShaftSFXUserData, soundData);
		};
		if (play)
		{
			if (soundData.turretSoundChannel == null)
			{
				soundData.turretSoundChannel = this->battlefieldModel.soundManager.playSound(sfxData.targetingSound, 0, 9999);
			};
		}
		else
		{
			if (soundData.turretSoundChannel != null)
			{
				this->battlefieldModel.soundManager.stopSound(soundData.turretSoundChannel);
				soundData.turretSoundChannel = null;
			};
		};
	}
	catch(e:Error)
	{
		alert = new AlertBugWindow();
		alert.text = ("Произошла ошибка: " + e.getStackTrace());
		Main.stage.addChild(alert);
	}
	*/
}

void ShaftSFXModel::createHitPointsGraphicEffects(ClientObjectWeapon *turretObject,
												  ClientObjectUser *owner,
												  const Vector3 *pos1,
												  const Vector3 *pos2,
												  Vector3 *param3,
												  Vector3 *param4,
												  Vector3 *param5)
{
	ShaftSFXData *sfxData = ClientObjectWeaponShaft::SWFData(turretObject);
	if (pos1 != nullptr)
	{
		this->createEffectsForPoint(turretObject, owner, sfxData, pos1, param3, param4, param5, false);
	}
	if (pos2 != nullptr)
	{
		this->createEffectsForPoint(turretObject, owner, sfxData, pos2, param3, param4, param5, true);
	}

}

void ShaftSFXModel::createEffectsForPoint(ClientObjectWeapon *turretObject,
										  ClientObjectUser *owner,
										  ShaftSFXData *sfxData,
										  const Vector3 *param1,
										  Vector3 *param2,
										  Vector3 *param3,
										  Vector3 *param4,
										  bool param5)
{
	double _loc6_ = sfxData->trailLength;

	Vector3 vectorToHitPoint;

	vectorToHitPoint.diff(param1, param2);
	if (vectorToHitPoint.dot(param3) > 0)
	{
		double _loc7_ = vectorToHitPoint.length();
		if (_loc7_ > _loc6_)
		{
			_loc7_ = _loc6_;
		};
		this->createTrailEffect1(sfxData, param1, param4, _loc7_, (_loc7_ / _loc6_));
		if (param5)
		{
			this->createTrailEffect2(sfxData, param1, param4, _loc7_, 0.5);
		}
	}
	this->createExplosionGraphicEffect(turretObject, owner, sfxData, param1);
	this->createExplosionSoundEffect(turretObject, owner, sfxData, param1);
}

void ShaftSFXModel::createExplosionGraphicEffect(ClientObjectWeapon *turretObject, ClientObjectUser *owner, ShaftSFXData *sfxData, const Vector3 *param1)
{
	(void)turretObject;
	(void)owner;
	auto effect1 = objectPoolService->getStaticObject3DPositionProvider();
	effect1->init(param1, EXPLOSION_OFFSET_TO_CAMERA);
	auto effect2 = objectPoolService->getAnimatedSpriteEffectNew();
	effect2->init(EXPLOSION_WIDTH * 3, 2.5 * EXPLOSION_WIDTH, sfxData->explosionAnimation, 0, effect1);
	this->battlefieldModel->addGraphicEffect(effect2);

}

void ShaftSFXModel::createExplosionSoundEffect(ClientObjectWeapon *turretObject, ClientObjectUser *owner, ShaftSFXData *sfxData, const Vector3 *param1)
{
	(void)turretObject;
	(void)owner;
	(void)sfxData;
	(void)param1;
	//var _loc2_:Sound3D = Sound3D.create(sfxData.explosionSound, 1000, 5000, 5, EXPLOSION_SOUND_VOLUME);
	//var _loc3_:Sound3DEffect = Sound3DEffect(objectPoolService.objectPool.getObject(Sound3DEffect));
	//_loc3_.init(owner, param1, _loc2_, 100);
	//this->battlefieldModel.addSound3DEffect(_loc3_);
}



void ShaftSFXModel::createTrailEffect1(ShaftSFXData *sfxData,
									   const Vector3 *position,
									   const Vector3 *direction,
									   double param4,
									   double param5)
{
	ShaftTrailEffect *effect = objectPoolService->getTrailEffect1();
	effect->init(position, direction, param4, param5, sfxData->trailMaterial, TRAIL_DURATION);
	this->battlefieldModel->addGraphicEffect(effect);
}

void ShaftSFXModel::createTrailEffect2(ShaftSFXData *sfxData,
									   const Vector3 *position,
									   const Vector3 *direction,
									   double param4,
									   double param5)
{
	ShaftTrailEffect *effect = objectPoolService->getTrailEffect2();
	effect->init(position, direction, param4, param5, sfxData->trailMaterial, TRAIL_DURATION);
	this->battlefieldModel->addGraphicEffect(effect);
}
