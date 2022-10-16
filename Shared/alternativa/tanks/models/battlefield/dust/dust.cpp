#include "dust.h"
#include "../battlefieldmodel.h"
#include "../battlefielddata.h"
#include "../battleview3d.h"
#include "randomgenerator.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/sfx/scalingobject3dpositionprovider.h"
#include "alternativa/tanks/sfx/limiteddistanceanimatedspriteeffect.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include <QHashIterator>
#include "alternativa/tanks/vehicles/tanks/track.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "alternativa/tanks/vehicles/tanks/suspensionray.h"
#include "mymath.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"


//static const double CHANCE = 0.2;
static const double SCALE_JITTER = 1;
static Vector3 bias(100, 0, 0);


static void addJitter(Vector3 *vector, double value)
{
	vector->x += (((RandomGenerator::random() - 0.5) * 2) * value);
	vector->y += (((RandomGenerator::random() - 0.5) * 2) * value);
	vector->z += (((RandomGenerator::random() - 0.5) * 2) * value);
}


Dust::Dust(BattlefieldModel *battlefieldModel)
{
	this->battleService = battlefieldModel;
	this->camera = battlefieldModel->bfData->viewport->camera;
	this->enabled = true;
}

void Dust::init(std::shared_ptr<BitmapData> &bitmapData, double farDistance, double nearDistance, double dustSize, double intensity, double density)
{

	this->farDistance = farDistance;
	this->nearDistance = nearDistance;
	this->dustSize = dustSize;
	this->intensity = intensity;
	this->density = density;

	animation = new TextureAnimation;
	GraphicsUtils::getTextureAnimation(animation, bitmapData, 32, 32);
	this->animation->fps = 30;
}

void Dust::addTank(TankData *tankData)
{
	this->tanks[tankData] = tankData->tank->getBoundSphereRadius() / 600;
}

void Dust::removeTank(TankData *tankData)
{
	this->tanks.remove(tankData);
}

void Dust::update()
{
	if ((this->enabled && this->camera->softTransparency) && (this->camera->softTransparencyStrength > 0))
	{
		QHashIterator<TankData*, double> i(this->tanks);
		while (i.hasNext())
		{
			i.next();
			TankData *tankData = i.key();

			if (tankData != nullptr && tankData->spawnState == TankSpawnState::ACTIVE && this->battleService->bfData->activeTanks->value(tankData))
			{
				this->addTankDust(tankData, 100, this->density);
			}
		}
	}
}

void Dust::addTankDust(TankData *tankData, double param2, double param3)
{
	double _loc4_ = this->tanks.value(tankData);
	Track *leftTrack = tankData->tank->leftTrack;
	Track *rightTrack = tankData->tank->rightTrack;
	if ((tankData->tank->leftThrottle * tankData->tank->rightThrottle) < 0)
	{
		param2 = 5;
	}



	Vector3 particleVelocity;

	Matrix3 *baseMatrix = &tankData->tank->baseMatrix;
	bias.x = bias.x * -1;
	baseMatrix->transformVector(&bias, &particleVelocity);
	this->addTrackDust(leftTrack, _loc4_, &particleVelocity, param2, param3, tankData->tank->leftThrottle);
	bias.x = bias.x * -1;
	baseMatrix->transformVector(&bias, &particleVelocity);
	this->addTrackDust(rightTrack, _loc4_, &particleVelocity, param2, param3, tankData->tank->rightThrottle);
}

void Dust::addTrackDust(Track *track, double param2, Vector3 *param3, double param4, double param5, double trackSpeed)
{
	for (uint32_t i = 0; i < track->raysNum; i++)
	{
		SuspensionRay *suspensionRay = track->rays[i];
		double _loc8_ = ((MyMath::abs(trackSpeed) / 1000) * 2);
		if (((_loc8_ > param4) && (RandomGenerator::random() < param5)) && (track->lastContactsNum > 0))
		{
			double _loc9_ = (_loc8_ > 500) ? 1 : (0.3 + (_loc8_ / 712));
			Vector3 particlePosition;
			particlePosition.copy(suspensionRay->getGlobalOrigin());
			addJitter(&particlePosition, 50);
			addJitter(param3, 20);
			this->createDustParticle(param2, &particlePosition, param3, _loc9_);
		}
	}
}


void Dust::createDustParticle(double param1, const Vector3 *param2, const Vector3 *param3, double param4)
{
	if ((this->enabled && (this->camera->softTransparency)) && (this->camera->softTransparencyStrength > 0))
	{

		ObjectPoolService *objectPoolService = this->battleService->getObjectPoolService();

		ScalingObject3DPositionProvider *scaling = objectPoolService->getScalingObject3DPositionProvider();
		scaling->init(param2, param3, 0.01);


		std::shared_ptr<ColorTransform> color;

		LimitedDistanceAnimatedSpriteEffect *limited = objectPoolService->getLimitedDistanceAnimatedSpriteEffect();
		double size = (this->dustSize * param1) * (1 + (SCALE_JITTER * RandomGenerator::random()));
		limited->init(size,
					  size,
					  this->animation,
					  (RandomGenerator::random() * 2) * MyMath::PI,
					  scaling,
					  0.5,
					  0.5,
					  color,
					  130,
					  BlendMode::NORMAL,
					  this->nearDistance,
					  this->farDistance,
					  (this->intensity * param4) * this->camera->softTransparencyStrength,
					  true);

		//return;

		this->battleService->addGraphicEffect(limited);
	}
}
