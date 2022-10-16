#include "railgunsfxmodel.h"
#include "alternativa/object/clientobjectweaponrailgun.h"
#include "service.h"
#include "alternativa/tanks/engine3d/tankstexturematerial.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/engine3d/core/MipMapping.h"
#include "beameffect.h"
#include "alternativa/tanks/models/sfx/animatedlighteffect.h"
#include "alternativa/tanks/models/sfx/muzzlepositionprovider.h"
#include "alternativa/tanks/models/sfx/lightdatamanager.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/models/sfx/tubelighteffect.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "chargeeffect.h"
#include "RailgunShootSFXData.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/sfx/colortransform/colortransformentry.h"
#include "flash/geom/colortransform.h"
#include "flash/geom/matrix.h"
#include "mymath.h"
#include "flash/geom/geomrectangle.h"
#include <QPainter>
#include <QImage>
#include <QDebug>


static const double MIPMAP_RESOLUTION = 3;
static const int CHARGE_FRAME_SIZE = 210;
//static const double SOUND_VOLUME = 1;





RailgunSFXModel::RailgunSFXModel()
{
	this->numLoadedObjects = 0;
	this->materialRegistry = Service::getIMaterialRegistry();
	this->bfModel = Service::getBattlefieldModel();
	this->objectPoolService = this->bfModel->getObjectPoolService();
	this->chargeLightEffect = nullptr;
	this->chargeMaterials = nullptr;
	this->charingData = nullptr;
	this->colorTransform = new ColorTransform();
}

RailgunSFXModel::~RailgunSFXModel()
{
	delete colorTransform;
}

void RailgunSFXModel::initObject(RailgunShootSFXData *SFXData, const QString &chargingPart1, const QString &chargingPart2, const QString &chargingPart3, const QString &railImageId, const QString &shotSoundId)
{
	(void)shotSoundId;

	this->numLoadedObjects++;

	if (this->numLoadedObjects == 1)
	{
		this->battlefield = Service::getIBattleField();
		std::shared_ptr<BitmapData> image = ResourceUtil::getResourceImage(railImageId)->bitmapData->data;
		this->trailMaterial = new_TanksTextureMaterial(image, true, true, MipMapping::OBJECT_DISTANCE, MIPMAP_RESOLUTION);
		this->trailMaterial->repeat = true;
		this->createChargeMaterials(chargingPart1, chargingPart2, chargingPart3);
		//this->shotSound = (ResourceUtil.getResource(ResourceType.SOUND, "railgun_shot").sound as Sound);
	}
	this->objectLoaded(SFXData);
}

IGraphicEffect* RailgunSFXModel::createGraphicShotEffect(ClientObjectWeapon *clientObject, const Vector3 *startPosition, const Vector3 *hitPosition)
{
	BeamEffect *effect = objectPoolService->getBeamEffect();
	std::shared_ptr<Material> material = this->trailMaterial;
	effect->init(startPosition, hitPosition, material, 25, 0.5, 1.5, 20, 1000);
	this->createShotLightEffect(startPosition, clientObject);
	this->createRailLightEffect(clientObject, startPosition, hitPosition, clientObject);
	return effect;
}


IGraphicEffect* RailgunSFXModel::createChargeEffect(ClientObjectWeapon *clientObject, ClientObjectUser *user, RailgunShootSFXData *sfxData, const Vector3 *localMuzzlePosition, Object3D *turret, int chargingTime)
{
	ChargeEffect *effect = objectPoolService->getChargeEffect();

	effect->init(user,
				 210,
				 210,
				 this->chargeMaterials,
				 localMuzzlePosition,
				 turret,
				 0,
				 (1000 * this->chargeMaterials->length()) / chargingTime,
				 sfxData->colorTransform);

	this->createChargeLightEffect(localMuzzlePosition, turret, chargingTime, clientObject);
	return effect;
}

void RailgunSFXModel::createChargeLightEffect(const Vector3 *param1, Object3D *param2, int param3, ClientObjectWeapon *turretObject)
{
	if (this->chargeLightEffect != nullptr)
	{
		this->chargeLightEffect->kill();
	}

	LightAnimation *a = LightDataManager::getLightDataMuzzle(turretObject->getId());
	if (a == nullptr)
	{
		qDebug() << "error getLightDataMuzzle" << turretObject->getId();
		return;
	}

	this->chargeLightEffect = objectPoolService->getAnimatedLightEffect();
	MuzzlePositionProvider *muzzle = objectPoolService->getMuzzlePositionProvider();
	muzzle->init(param2, param1);
	this->chargeLightEffect->initFromTime(muzzle, param3, a);
	this->bfModel->addGraphicEffect(this->chargeLightEffect);
}

void RailgunSFXModel::createRailLightEffect(ClientObjectWeapon *clientObject, const Vector3 *param1, const Vector3 *param2, ClientObjectWeapon *turretObject)
{
	(void)clientObject;

	StaticObject3DPositionProvider *_loc3_ = objectPoolService->getStaticObject3DPositionProvider();
	StaticObject3DPositionProvider *_loc4_ = objectPoolService->getStaticObject3DPositionProvider();
	_loc3_->init(param1, 0);
	_loc4_->init(param2, 0);
	TubeLightEffect *_loc5_ = objectPoolService->getTubeLightEffect();
	_loc5_->init(_loc3_, _loc4_, LightDataManager::getLightDataShot(turretObject->getId()));
	this->bfModel->addGraphicEffect(_loc5_);
}

void RailgunSFXModel::createShotLightEffect(const Vector3 *param1, ClientObjectWeapon *turretObject)
{
	AnimatedLightEffect *_loc2_ = objectPoolService->getAnimatedLightEffect();
	StaticObject3DPositionProvider *_loc3_ = objectPoolService->getStaticObject3DPositionProvider();
	_loc3_->init(param1, 0);
	_loc2_->init(_loc3_, LightDataManager::getLightDataExplosion(turretObject->getId()));
	this->bfModel->addGraphicEffect(_loc2_);
}

ISound3DEffect* RailgunSFXModel::createSoundShotEffect(ClientObject *clientObject, ClientObject *effectOwner, const Vector3 *startGlobalPos)
{
	(void)clientObject;
	(void)effectOwner;
	(void)startGlobalPos;
#if 0
	if (this->shotSound == null)
		return null;

	Sound3D sound = Sound3D.create(this->shotSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, SOUND_VOLUME);
	return (Sound3DEffect.create(objectPoolService.objectPool, effectOwner, startGlobalPos, sound));
#endif
	return nullptr;
}

void RailgunSFXModel::objectLoaded(RailgunShootSFXData *SFXData)
{
	(void)SFXData;

	/*
	var modelService:IModelService = IModelService(Main.osgi.getService(IModelService));
	IColorTransformModel colorTransformModel = IColorTransformModel(modelService.getModelForObject(clientObject, IColorTransformModel));
	if (colorTransformModel != nullptr)
	{
		QVector<ColorTransformEntry> ctVector = colorTransformModel.getModelData(clientObject);
		if (ctVector != nullptr && ctVector.length > 0)
		{
			ColorTransformEntry *ctData = ctVector[0];
			sfxData->colorTransform = std::make_shared<ColorTransform>(ctData->redMultiplier,
																	   ctData->greenMultiplier,
																	   ctData->blueMultiplier,
																	   ctData->alphaMultiplier,
																	   ctData->redOffset,
																	   ctData->greenOffset,
																	   ctData->blueOffset,
																	   ctData->alphaOffset);
		}
	}
	*/


}

void RailgunSFXModel::objectUnloaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;

	this->numLoadedObjects--;
	if (this->numLoadedObjects == 0)
	{
		this->trailMaterial = nullptr;
		this->chargeMaterials = nullptr;
		//this->shotSound = nullptr;
	}
}



void RailgunSFXModel::createChargeMaterials(const QString &chargingPart1, const QString &chargingPart2, const QString &chargingPart3)
{
	(void)chargingPart1;
	(void)chargingPart2;
	(void)chargingPart3;

	std::shared_ptr<BitmapData> part1 = ResourceUtil::getResourceImage(chargingPart1)->bitmapData->data;
	if (part1 == nullptr)
		throw 1343432413;

	std::shared_ptr<BitmapData> part2 = ResourceUtil::getResourceImage(chargingPart2)->bitmapData->data;
	if (part2 == nullptr)
		throw 1343432414;

	std::shared_ptr<BitmapData> part3 = ResourceUtil::getResourceImage(chargingPart3)->bitmapData->data;
	if (part3 == nullptr)
		throw 1343432415;



	std::shared_ptr<BitmapData> chargeTexture = this->createChargeTexture(part1.get(), part2.get(), part3.get());

	this->charingData = new TextureAnimation;

	GraphicsUtils::getTextureAnimation(this->charingData, chargeTexture, 210, 210);
	this->charingData->fps = 30;
	this->chargeMaterials = materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
																					  chargeTexture.get(),
																					  CHARGE_FRAME_SIZE,
																					  MIPMAP_RESOLUTION,
																					  true)->materials;
}

std::shared_ptr<BitmapData> RailgunSFXModel::createChargeTexture(BitmapData *chargingPart1, BitmapData *chargingPart2, BitmapData *chargingPart3)
{
	int numFrames = 30;
	std::shared_ptr<BitmapData> texture = std::make_shared<BitmapData>((CHARGE_FRAME_SIZE * numFrames), CHARGE_FRAME_SIZE, true, 0);
	texture->fill(0x00000000);

	for (int i = 1; i <= numFrames; i++)
	{
		this->drawPart1(chargingPart1, texture.get(), i, CHARGE_FRAME_SIZE);
		this->drawPart2(chargingPart2, texture.get(), i, CHARGE_FRAME_SIZE);
		this->drawPart3(chargingPart3, texture.get(), i, CHARGE_FRAME_SIZE);
	}

	return texture;
}

void RailgunSFXModel::drawPart1(BitmapData *sourceBitmapData, BitmapData *destBitmapData, int frame, int frameSize)
{
	if (frame < 15)
	{
		this->colorTransform->alphaMultiplier = (frame - 1) / 14.;
	}
	else
	{
		if (frame < 26)
		{
			this->colorTransform->alphaMultiplier = 1;
		}
		else
		{
			this->colorTransform->alphaMultiplier = 1 - ((frame - 25) / 5.);
		}
	}


	Matrix matrix;
	matrix.identity();
	matrix.tx = ((frame - 1) * frameSize) + (0.5 * (frameSize - sourceBitmapData->width()));
	matrix.ty = 0.5 * (frameSize - sourceBitmapData->height());
	destBitmapData->draw(sourceBitmapData, &matrix, nullptr, BlendMode::NORMAL, nullptr, true);
}

void RailgunSFXModel::drawPart2(BitmapData *sourceBitmapData, BitmapData *destBitmapData, int frame, int frameSize)
{
	if (frame < 6)
	{
		this->colorTransform->alphaMultiplier = ((frame - 1) / 5.);
	}
	else
	{
		if (frame < 26)
		{
			this->colorTransform->alphaMultiplier = 1;
		}
		else
		{
			this->colorTransform->alphaMultiplier = (1 - ((frame - 25) / 5.));
		}
	}


	int w = sourceBitmapData->width();
	int h = sourceBitmapData->height();

	QPainter p(destBitmapData->qimage());
	p.setRenderHint(QPainter::Antialiasing);
	p.translate(((frame - 1) * frameSize) + (0.5 * frameSize), 0.5 * frameSize);
	p.rotate(2 * (frame - 1));
	p.translate(-0.5 * w, -0.5 * h);
	p.drawImage(0, 0, *sourceBitmapData->qimage(), 0, 0, w, h);
	p.end();

	//Matrix matrix;
	//matrix.identity();
	//matrix.translate(-0.5 * sourceBitmapData->width(), -0.5 * sourceBitmapData->height());
	//matrix.rotate(((2 * (frame - 1)) * MyMath::PI) / 180);
	//matrix.translate(((frame - 1) * frameSize) + (0.5 * frameSize), 0.5 * frameSize);
	//destBitmapData->draw(sourceBitmapData, &matrix, this->colorTransform, BlendMode::NORMAL, nullptr, true);
}

void RailgunSFXModel::drawPart3(BitmapData *sourceBitmapData, BitmapData *destBitmapData, int frame, int frameSize)
{
	double scale;
	if (frame < 25)
	{
		double k = (frame - 1) / 24.;
		this->colorTransform->alphaMultiplier = k;
		scale = 0.4 + (0.6 * k);
	}
	else
	{
		if (frame < 26)
		{
			this->colorTransform->alphaMultiplier = 1;
			scale = 1;
		}
		else
		{
			double k = 1 - ((frame - 25) / 5.);
			this->colorTransform->alphaMultiplier = k;
			scale = 0.2 + (0.8 * k);
		}
	}

	int w = sourceBitmapData->width();
	int h = sourceBitmapData->height();

	QPainter p(destBitmapData->qimage());
	p.setRenderHint(QPainter::Antialiasing); // сглаживание
	p.translate(((frame - 1) * frameSize) + (0.5 * frameSize), 0.5 * frameSize);
	p.scale(scale, scale);
	p.rotate(2 * (frame - 1));
	p.translate(-0.5 * w, -0.5 * h);//надо так, а то вращается не вокруг центра
	p.drawImage(0, 0, *sourceBitmapData->qimage(), 0, 0, w, h);
	p.end();

	//Matrix matrix;
	//matrix.identity();
	//matrix.translate(-0.5 * sourceBitmapData->width(), -0.5 * sourceBitmapData->height());
	//matrix.scale(scale, scale);
	//matrix.rotate(((2 * (1 - frame)) * MyMath::PI) / 180);
	//matrix.translate(((frame - 1) * frameSize) + (0.5 * frameSize), (0.5 * frameSize));
	//destBitmapData->draw(sourceBitmapData, &matrix, this->colorTransform, BlendMode::NORMAL, nullptr, true);
}
