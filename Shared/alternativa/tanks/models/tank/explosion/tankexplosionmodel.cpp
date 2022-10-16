#include "tankexplosionmodel.h"
#include "explosiondata.h"
#include "flash/display/bitmapdata.h"
#include "resource/resourceutil.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "service.h"
#include "mymath.h"
#include "randomgenerator.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/sfx/animatedplaneeffectold.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/tanks/sfx/movingobject3dpositionprovider.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/object/clientobjecthull.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"



static const double EXPLOSION_SIZE = 800;
static const double SMOKE_SIZE = 400;
static const double SHOCKWAVE_SIZE = 1000;
static const double BASE_DIAGONAL = 600;
static const double MIN_SMOKE_SPEED = 800;
static const double SMOKE_SPEED_DELTA = 200;
static const double SMOKE_ACCELERATION = -2000;
static const double EXPLOSION_FPS = 25;
static const double SMOKE_FPS = 15;

static const double shockWaveScaleSpeed = 1;
static const double smokeSize = SMOKE_SIZE;
static const double explosionSize = EXPLOSION_SIZE;
static const double scaleModifier = 1;

/*
private var objectPoolService:IObjectPoolService = IObjectPoolService(Main.osgi.getService(IObjectPoolService));
private var materialRegistry:IMaterialRegistry = IMaterialRegistry(Main.osgi.getService(IMaterialRegistry));
private var explosionSize:ConsoleVarFloat = new ConsoleVarFloat("tankexpl_size", EXPLOSION_SIZE, 1, 2000);
private var smokeSize:ConsoleVarFloat = new ConsoleVarFloat("tankexpl_smoke_size", SMOKE_SIZE, 1, 2000);
private var scaleModifier:ConsoleVarFloat = new ConsoleVarFloat("tankexpl_scale", 1, 0, 10);
private var shockWaveScaleSpeed:ConsoleVarFloat = new ConsoleVarFloat("tankexpl_scale_speed", 1, 0, 10);
private var battlefieldModel:IBattleField;
private var rayHit:RayIntersection = new RayIntersection();
private var position:Vector3 = new Vector3();
private var eulerAngles:Vector3 = new Vector3();
private var velocity:Vector3 = new Vector3();
private var matrix:Matrix3 = new Matrix3();
*/



static ExplosionData* getData(ClientObjectHull *clientObject)
{
	return clientObject->tankExplosionModel.get();
	//return (ExplosionData(clientObject.getParams(TankExplosionModel)));
}

static MaterialSequence* getMaterialSequence(const QString &textureId, double size)
{
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();
	BitmapData *texture = ResourceUtil::getResourceImage(textureId)->bitmapData->data.get();
	int frameSize = texture->height();
	return materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT, texture, frameSize, size / frameSize);
}


bool TankExplosionModel::initObject(ClientObjectHull *clientObject, const QString &explosionTextureId, const QString &shockWaveTextureId, const QString &smokeTextureId)
{
	/*
	var modelService:IModelService;
	{
		modelService = IModelService(Main.osgi.getService(IModelService));
	}
	*/

	MaterialSequence *explosionSequence = getMaterialSequence(explosionTextureId, EXPLOSION_SIZE);
	MaterialSequence *shockWaveSequence = getMaterialSequence(shockWaveTextureId, SHOCKWAVE_SIZE);
	MaterialSequence *smokeSequence = getMaterialSequence(smokeTextureId, SMOKE_SIZE);
	if (explosionSequence == nullptr || shockWaveSequence == nullptr || smokeSequence == nullptr)
		return false;

	std::shared_ptr<TextureAnimation> explData = std::make_shared<TextureAnimation>();
	std::shared_ptr<TextureAnimation> shockData = std::make_shared<TextureAnimation>();
	std::shared_ptr<TextureAnimation> smokeData = std::make_shared<TextureAnimation>();

	GraphicsUtils::getTextureAnimation(explData.get(), ResourceUtil::getResourceImage(explosionTextureId)->bitmapData->data, 200, 200);
	explData->fps = 25;
	GraphicsUtils::getTextureAnimation(shockData.get(), ResourceUtil::getResourceImage(shockWaveTextureId)->bitmapData->data, 250, 250);
	shockData->fps = 25;
	GraphicsUtils::getTextureAnimation(smokeData.get(), ResourceUtil::getResourceImage(smokeTextureId)->bitmapData->data, 200, 200);
	smokeData->fps = 15;

	std::shared_ptr<ExplosionData> explosionSFXData = std::make_shared<ExplosionData>(explosionSequence, shockWaveSequence, smokeSequence);
	explosionSFXData->explosionData = explData;
	explosionSFXData->shockData = shockData;
	explosionSFXData->smokeData = smokeData;
	clientObject->tankExplosionModel = explosionSFXData;

	return true;
}


void TankExplosionModel::objectUnloaded(ClientObjectHull *object)
{
	getData(object)->release();
}


void TankExplosionModel::createExplosionEffects(ClientObjectHull *clientObject, TankData *tankData)
{
	IBattleField *battlefieldModel = Service::getIBattleField();


	ExplosionData *explosionData = getData(clientObject);
	if (explosionData == nullptr)
	{
		return;
	}

	ObjectPoolService *poolService = battlefieldModel->getObjectPoolService();




	//ObjectPool *objectPool = this.objectPoolService.objectPool;
	Mesh *hullMesh = tankData->tank->skin->hullMesh;
	double dx = hullMesh->bound.MaxX - hullMesh->bound.MinX;
	double dy = hullMesh->bound.MaxY - hullMesh->bound.MinY;
	double dz = hullMesh->bound.MaxZ - hullMesh->bound.MinZ;
	double diagonal = MyMath::sqrt((dx * dx) + (dy * dy) + (dz * dz));
	double effectScale = (scaleModifier * diagonal) / BASE_DIAGONAL;
	Vector3 dir(0, 0, -1);
	double maxTime = 500;
	Vector3 position;
	position.copy(&tankData->tank->state.pos);
	RayIntersection rayHit;

	if (battlefieldModel->getBattlefieldData()->collisionDetector->intersectRayWithStatic(&position, &dir, 0xFF, maxTime, nullptr, &rayHit))
	{
		rayHit.pos.z = rayHit.pos.z + 10;
		double size = SHOCKWAVE_SIZE;
		double minTime = 200;
		if (rayHit.t > minTime)
		{
			size = size * ((maxTime - rayHit.t) / (maxTime - minTime));
		}
		const Vector3 *normal = &rayHit.normal;
		double angle = MyMath::acos(normal->z);
		Vector3 axis(-normal->y, normal->x, 0);
		axis.normalize();
		Matrix3 matrix;
		Vector3 eulerAngles;
		matrix.fromAxisAngle(&axis, angle);
		matrix.getEulerAngles(&eulerAngles);

		AnimatedPlaneEffectOld *animatedPlaneEffect = poolService->getAnimatedPlaneEffectOld();

		animatedPlaneEffect->init(effectScale * size,
								  &rayHit.pos,
								  &eulerAngles,
								  EXPLOSION_FPS,
								  explosionData->shockWaveSequence->materials,
								  shockWaveScaleSpeed);


		//QVector<std::shared_ptr<Material>> *materials
		//double size, const Vector3 *position, const Vector3 *rotation, double fps, QVector<Material *> frames, double scaleSpeed

		battlefieldModel->addGraphicEffect(animatedPlaneEffect);
	}
	position.z = position.z + 50;
	AnimatedSpriteEffectNew *explosion = poolService->getAnimatedSpriteEffectNew();
	TextureAnimation *animaton = explosionData->explosionData.get();
	StaticObject3DPositionProvider *pos = poolService->getStaticObject3DPositionProvider();
	pos->init(&position, 200);
	double explosionSize = ::explosionSize * effectScale;
	MaterialSequence *explosionSequence = explosionData->explosionSequence;
	explosionSequence->setMipMapResolution(explosionSize / explosionSequence->frameWidth());
	explosion->init(explosionSize, explosionSize, animaton, (RandomGenerator::random() * 2) * MyMath::PI, pos);
	battlefieldModel->addGraphicEffect(explosion);

	for (int i = 0; i < 3; i++)
	{
		double speed = MIN_SMOKE_SPEED + (RandomGenerator::random() * SMOKE_SPEED_DELTA);
		Vector3 velocity;
		velocity.x = speed * (1 - (2 * RandomGenerator::random()));
		velocity.y = speed * (1 - (2 * RandomGenerator::random()));
		velocity.z = (speed * 0.5) * (1 + RandomGenerator::random());
		AnimatedSpriteEffectNew *smokeEffect = poolService->getAnimatedSpriteEffectNew();
		TextureAnimation *animationSmoke = explosionData->smokeData.get();
		MovingObject3DPositionProvider *positionProvider = poolService->getMovingObject3DPositionProvider();
		positionProvider->init(&position, &velocity, -2000);
		double smokeSize = ::smokeSize * effectScale;
		smokeEffect->init(smokeSize, smokeSize, animationSmoke, (RandomGenerator::random() * 2) * MyMath::PI, positionProvider);
		battlefieldModel->addGraphicEffect(smokeEffect);
	}
}

