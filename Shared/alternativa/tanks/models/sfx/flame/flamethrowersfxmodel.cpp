#include "flamethrowersfxmodel.h"
#include "alternativa/tanks/models/sfx/flame/flamethrowersfxdata.h"
#include "service.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/tanks/sfx/effectspair.h"
#include "alternativa/object/clientobjectweaponflamethrower.h"
#include "com/alternativaplatform/projects/tanks/client/warfare/models/colortransform/struct/colortransformstruct.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/weapon/flamethrower/flamethrowermodel.h"
#include "flamethrowergraphiceffect.h"
#include "alternativa/tanks/models/weapon/flamethrower/FlamethrowerData.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/tanks/models/sfx/colortransform/colortransformentry.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "battlecontroller.h"


static const int MIPMAP_RESOLUTION = 3;
//private static const SOUND_VOLUME:Number = 1;
//private static const colorTarnsforms:Vector.<ColorTransformEntry> = Vector.<ColorTransformEntry>([new ColorTransformEntry(new ColorTransformStruct(0, 1, 1, 1, 1, 100, 150, 100, 0)), new ColorTransformEntry(new ColorTransformStruct(0.05, 1, 1, 1, 1, 50, 100, 60, 0)), new ColorTransformEntry(new ColorTransformStruct(0.1, 1, 1, 1, 1, 100, 100, 40, 0)), new ColorTransformEntry(new ColorTransformStruct(0.65, 0.5, 0.3, 0.3, 1, 50, 80, 50, 0)), new ColorTransformEntry(new ColorTransformStruct(0.75, 0, 0, 0, 1, 50, 50, 50, 0)), new ColorTransformEntry(new ColorTransformStruct(1, 0, 0, 0, 0, 20, 20, 20, 0))]);


static FlamethrowerSFXData* getData(ClientObjectWeapon *clientObject)
{
	return ClientObjectWeaponFlamethrower::SWFData(clientObject);
}

FlamethrowerSFXModel::FlamethrowerSFXModel()
{
	this->battlefield = nullptr;
	this->flamethrowerModel = Service::getBattleController()->flamethrowerModel;
	this->materialRegistry = Service::getIMaterialRegistry();
}

FlamethrowerSFXModel::~FlamethrowerSFXModel()
{

}


void FlamethrowerSFXModel::initObject(ClientObjectWeapon *clientObject, const QString &fireTextureId, const QString &flameSoundId)
{
	(void)flameSoundId;

	if (this->battlefield == nullptr)
		this->battlefield = Service::getIBattleField();

	std::shared_ptr<FlamethrowerSFXData> data = std::make_shared<FlamethrowerSFXData>();
	std::shared_ptr<BitmapData> fireTexture = ResourceUtil::getResourceImage(fireTextureId)->bitmapData->data;

	data->materials = materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT,
																				fireTexture.get(),
																				fireTexture->height(),
																				MIPMAP_RESOLUTION)->materials;

	//data->flameSound = (ResourceUtil.getResource(ResourceType.SOUND, flameSoundId).sound as Sound);
	data->data = std::make_shared<TextureAnimation>();
	GraphicsUtils::getTextureAnimation(data->data.get(), fireTexture, 100, 100);
	data->data->fps = 30;
	ClientObjectWeaponFlamethrower::setSWFData(clientObject, data);
}


void FlamethrowerSFXModel::getSpecialEffects(TankData *shooterData, const Vector3 *localMuzzlePosition, Object3D *turret, IWeaponWeakeningModel *weakeningModel, EffectsPair *effectsPair)
{
	FlamethrowerData *data = flamethrowerModel->getFlameData(shooterData->turret);
	FlamethrowerSFXData *sfxData = getData(shooterData->turret);

	FlamethrowerGraphicEffect *graphicEffect = battlefield->getObjectPoolService()->getFlamethrowerGraphicEffect();

	graphicEffect->init(shooterData, data->range, data->coneAngle, 20, 2000, localMuzzlePosition, turret, sfxData, this->battlefield->getBattlefieldData()->collisionDetector, weakeningModel);
	//Sound3D sound3D = Sound3D.create(sfxData.flameSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, SOUND_VOLUME);
	//MobileSound3DEffect soundEffect = MobileSound3DEffect(objectPoolService.objectPool.getObject(MobileSound3DEffect));
	//soundEffect.init(null, sound3D, turret, 0, 4);
	effectsPair->graphicEffect = graphicEffect;
	effectsPair->soundEffect = nullptr;
}


static QVector<ColorTransformEntry*> getColors()
{
	QVector<ColorTransformEntry*> list;

	static ColorTransformEntry *colors[6];
	if (colors[0] == nullptr)
	{
		colors[0] = new ColorTransformEntry(ColorTransformStruct(0, 1, 1, 1, 1, 100, 150, 100, 0));
		colors[1] = new ColorTransformEntry(ColorTransformStruct(0.05, 1, 1, 1, 1, 50, 100, 60, 0));
		colors[2] = new ColorTransformEntry(ColorTransformStruct(0.1, 1, 1, 1, 1, 100, 100, 40, 0));
		colors[3] = new ColorTransformEntry(ColorTransformStruct(0.65, 0.5, 0.3, 0.3, 1, 50, 80, 50, 0));
		colors[4] = new ColorTransformEntry(ColorTransformStruct(0.75, 0, 0, 0, 1, 50, 50, 50, 0));
		colors[5] = new ColorTransformEntry(ColorTransformStruct(1, 0, 0, 0, 0, 20, 20, 20, 0));
	}

	list.append(colors[0]);
	list.append(colors[1]);
	list.append(colors[2]);
	list.append(colors[3]);
	list.append(colors[4]);
	list.append(colors[5]);

	return list;
}


void FlamethrowerSFXModel::objectLoaded(ClientObjectWeapon *clientObject)
{
	FlamethrowerSFXData *sfxData = getData(clientObject);
	sfxData->colorTransformPoints = getColors();
}

void FlamethrowerSFXModel::objectUnloaded(ClientObjectWeapon *clientObject)
{
	(void)clientObject;
}
