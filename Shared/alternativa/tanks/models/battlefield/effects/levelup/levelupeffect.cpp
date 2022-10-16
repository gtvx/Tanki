#include "levelupeffect.h"
#include "mymath.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "lightbeameffect.h"
#include "service.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "resource/resourceutil.h"
#include "flash/display/bitmapdata.h"
#include "randomgenerator.h"
#include "sparkeffect.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "lightwaveeffect.h"
#include <QHash>


static QHash<int, std::shared_ptr<TextureMaterial>> textures;

static double getAvailableHeight(double param1, double param2, double param3, double param4)
{
	(void)param1;
	(void)param2;
	(void)param3;
	(void)param4;
	return 2000;
}


static double getEffectHeight(double x, double y, double z)
{
	double value = getAvailableHeight(x, y, z, 2000);
	return MyMath::max(500, value);
}




static void createLightBeams(BattlefieldModel *battlefield, double maxHeight, Object3D *object)
{
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	std::shared_ptr<BitmapData> beamBitmapData = ResourceUtil::getResourceImageLocal("level_up_effect/BeamTexture.png");
	if (beamBitmapData == nullptr)
		return;

	ObjectPoolService *pool = battlefield->getObjectPoolService();

	double value = 0;
	double step = (MyMath::PI * 2) / 6;
	TextureMaterial* texture = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT,
																					   beamBitmapData,
																					   beamBitmapData->height() / beamBitmapData->width(),
																					   false);
	for (int i = 0; i < 6; i++)
	{
		double x = MyMath::sin(value) * 90;
		double y = MyMath::cos(value) * 90;
		LightBeamEffect *effect = pool->getLightBeamEffect();
		effect->init(500, 200, 30, maxHeight, 0.8, 0.5, x, y, -50, object, texture);
		battlefield->addGraphicEffect(effect);
		value = value + step;
	}
}



static void createLabel(BattlefieldModel *battlefield, double value, int rang, Object3D *object)
{
	std::shared_ptr<TextureMaterial> texture = textures.value(rang);

	if (texture == nullptr)
	{
		std::shared_ptr<BitmapData> icon = ResourceUtil::getResourceImageLocal("ranks_normal/" + QString::number(rang) + ".png");
		if (icon == nullptr)
			return;

		texture = new_TextureMaterial();
		texture->init(false, true, MipMapping::PER_PIXEL, 1);
		texture->setTexture(icon);

		textures.insert(rang, texture);
	}


	ObjectPoolService *pool = battlefield->getObjectPoolService();
	SparkEffect *effect = pool->getSparkEffect();
	effect->init(500, 270, 270, 0, (value * 0.8), (value * 0.15), 0.35, 0, 0, 50, object, texture.get(), BlendMode::NORMAL);
	battlefield->addGraphicEffect(effect);
}



static void createSparks(BattlefieldModel *battlefield, double value, Object3D *object)
{
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	std::shared_ptr<BitmapData> sparkBitmapData = ResourceUtil::getResourceImageLocal("level_up_effect/SparkTexture.png");
	if (sparkBitmapData == nullptr)
		return;

	ObjectPoolService *pool = battlefield->getObjectPoolService();

	double _loc4_ = 100;
	TextureMaterial *texture = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT,
																					   sparkBitmapData,
																					   sparkBitmapData->height() / sparkBitmapData->width(),
																					   false);
	for (int i = 0; i < 15; i++)
	{
		double r = (MyMath::PI * 2) * RandomGenerator::random();
		double x = MyMath::sin(r) * _loc4_;
		double y = MyMath::cos(r) * _loc4_;
		double z = (-110 * i) - 50;
		SparkEffect *effect = pool->getSparkEffect();
		effect->init(400, 150, 150, r, (value * 0.7), (value * 0.15), 0.7, x, y, z, object, texture, BlendMode::ADD);
		battlefield->addGraphicEffect(effect);
	}
}


static void createWave(BattlefieldModel *battlefield, Object3D *object)
{
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();

	std::shared_ptr<BitmapData> waveBitmapData = ResourceUtil::getResourceImageLocal("level_up_effect/WaveTexture.png");
	if (waveBitmapData == nullptr)
		return;

	ObjectPoolService *pool = battlefield->getObjectPoolService();

	TextureMaterial *material = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT,
																					   waveBitmapData,
																					   waveBitmapData->height() / waveBitmapData->width(),
																					   false);


	LightWaveEffect *effect = pool->getLightWaveEffect();
	effect->init(900, 220, 3, true, object, material);
	battlefield->addGraphicEffect(effect);
}


void LevelUpEffect::createEffect(BattlefieldModel *battlefield, Tank *tank, int rang)
{
	Object3D* object = tank->skin->turretMesh;
	double value = getEffectHeight(object->position.x, object->position.y, object->position.z);
	createLightBeams(battlefield, value, object);
	createLabel(battlefield, value, rang, object);
	createSparks(battlefield, value, object);
	createWave(battlefield, object);
}
