#include "tankcriticalhitmodel.h"
#include "alternativa/math/vector3.h"
#include "randomgenerator.h"
#include "mymath.h"
#include "service.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/tanks/models/tank/explosion/explosiondata.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "alternativa/object/clientobjecthull.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "__global.h"


static const double EXPLOSION_SIZE = 800;
//private const SMOKE_SIZE:Number = 400;
//private const SHOCKWAVE_SIZE:Number = 1000;
static const double BASE_DIAGONAL = 600;
//private const MIN_SMOKE_SPEED:Number = 800;
//private const SMOKE_SPEED_DELTA:Number = 200;
//private const SMOKE_ACCELERATION:Number = -2000;
//private const EXPLOSION_FPS:Number = 25;
//private const SMOKE_FPS:Number = 15;

static const double explosionSize = EXPLOSION_SIZE;
static const double scaleModifier = 1;





static ExplosionData* getData(ClientObjectHull *clientObject)
{
	return clientObject->tankCriticalHitModel.get();
}

static MaterialSequence* getMaterialSequence(const QString &textureId, double size)
{
	std::shared_ptr<BitmapData> texture = ResourceUtil::getResourceImage(textureId)->bitmapData->data;
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();
	int frameSize = texture->height();
	return materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT, texture.get(), frameSize, (double(size) / double(frameSize)));
}

TankCriticalHitModel::TankCriticalHitModel()
{

}

bool TankCriticalHitModel::initObject(ClientObjectHull *clientObject,
									  const QString &explosionTextureId,
									  const QString &shockWaveTextureId,
									  const QString &smokeTextureId)
{
	(void)shockWaveTextureId;
	(void)smokeTextureId;

	/*
	var modelService:IModelService;
	if (this.battlefieldModel == null)
	{
		modelService = IModelService(Main.osgi.getService(IModelService));
		this.battlefieldModel = (Main.osgi.getService(IBattleField) as IBattleField);
	}
	*/

	MaterialSequence *explosionSequence = getMaterialSequence(explosionTextureId, EXPLOSION_SIZE);
	if (explosionSequence == nullptr)
		return false;


	std::shared_ptr<BitmapData> criticalHit;
#ifdef L_MT
	criticalHit = ResourceUtil::getResourceImage("criticalHitTexture")->bitmapData->data;
#endif

#if (L_GT == 1 || L_PRO)
	criticalHit = ResourceUtil::getResourceImageLocal("criticalhit/criticalHit.png");
#endif

	std::shared_ptr<TextureAnimation> explData = std::make_shared<TextureAnimation>();
	GraphicsUtils::getTextureAnimation(explData.get(), criticalHit, 200, 200);
	explData->fps = 25;
	std::shared_ptr<ExplosionData> explosionSFXData = std::make_shared<ExplosionData>(explosionSequence, nullptr, nullptr);
	explosionSFXData->explosionData = explData;
	clientObject->tankCriticalHitModel = explosionSFXData;

	return true;
}

void TankCriticalHitModel::objectLoaded(ClientObjectHull *object)
{
	(void)object;
}

void TankCriticalHitModel::objectUnloaded(ClientObjectHull *object)
{
	getData(object)->release();
}



void TankCriticalHitModel::createExplosionEffects(ClientObjectHull *clientObject, TankData *tankData)
{
	ExplosionData *explosionData = getData(clientObject);
	Mesh *hullMesh = tankData->tank->skin->hullMesh;
	double dx = (hullMesh->bound.MaxX - hullMesh->bound.MinX);
	double dy = (hullMesh->bound.MaxY - hullMesh->bound.MinY);
	double dz = (hullMesh->bound.MaxZ - hullMesh->bound.MinZ);
	double diagonal = MyMath::sqrt((((dx * dx) + (dy * dy)) + (dz * dz)));
	double effectScale = (::scaleModifier * diagonal) / BASE_DIAGONAL;


	IBattleField *battlefieldModel = Service::getIBattleField();
	ObjectPoolService *objectPoolService = battlefieldModel->getObjectPoolService();

	Vector3 position;

	position.copy(&tankData->tank->state.pos);
	position.z = position.z + 50;
	AnimatedSpriteEffectNew *explosion = objectPoolService->getAnimatedSpriteEffectNew();
	TextureAnimation *animaton = explosionData->explosionData.get();

	StaticObject3DPositionProvider *pos = objectPoolService->getStaticObject3DPositionProvider();
	pos->init(&position, 200);

	double explosionSize = ::explosionSize * effectScale;
	MaterialSequence *explosionSequence = explosionData->explosionSequence;
	explosionSequence->setMipMapResolution(explosionSize / explosionSequence->frameWidth());
	explosion->init(400, 400, animaton, (RandomGenerator::random() * 2) * MyMath::PI, pos);
	battlefieldModel->addGraphicEffect(explosion);
}
