#include "healinggungraphiceffect.h"
#include "com/alternativaplatform/projects/tanks/client/warfare/models/tankparts/weapon/healing/IsisActionType.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/models/sfx/muzzlepositionprovider.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/sfx/healing/IHealingGunEffectListener.h"
#include "alternativa/tanks/sfx/sfxutils.h"
#include "alternativa/tanks/models/sfx/healing/HealingGunSFXData.h"
#include "../omnistreamlighteffect.h"
#include"service.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "../lightdatamanager.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/object/clientobjectweapon.h"
#include "alternativa/tanks/models/weapon/healing/healinggunshaft.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/sfx/tubelighteffect.h"
#include "randomgenerator.h"


//static var endPositionOffset:ConsoleVarFloat = new ConsoleVarFloat("izida_end_offset", 150, 0, 500);
//static var turretMatrix:Matrix4 = new Matrix4();
//static var targetMatrix:Matrix4 = new Matrix4();
//static var endPosition:Vector3 = new Vector3();
//static var startPosition:Vector3 = new Vector3();
//static var direction:Vector3 = new Vector3();
//static var cameraPosition:Vector3 = new Vector3();
//static var objectPoolService:IObjectPoolService = IObjectPoolService(Main.osgi.getService(IObjectPoolService));

static const double endPositionOffset = 150;



static Material* getRandomFrame(QVector<std::shared_ptr<Material>>*  materials)
{
	return materials->at(int((RandomGenerator::random() * materials->length()))).get();
}



HealingGunGraphicEffect::HealingGunGraphicEffect(IHealingGunEffectListener *listener)
{
	this->bfModel = Service::getIBattleField();

	this->localTargetPosition = new Vector3();
	this->localSourcePosition = new Vector3();
	this->startPosition = new Vector3();

	this->beamEffect = nullptr;
	this->lightEffect = nullptr;
	this->container = nullptr;
	this->listener = listener;
	this->shaftPlane = new_HealingGunShaft();
	this->shaftPlane->init(100, 100);
	this->spark = new_AnimatedSprite3D(120, 120);
	this->shaftEnd = new_AnimatedSprite3D(200, 200);
	this->spark->softAttenuation = 0;
	this->shaftEnd->softAttenuation = 0;
	this->shaftEnd->originY = 0.65;
	this->shaftEndFrame = 0;
}

void HealingGunGraphicEffect::createGraphics()
{
	if (this->shaftPlane == nullptr)
	{
		this->shaftPlane = new_HealingGunShaft();
		this->shaftPlane->init(100, 100);
	}
	if (this->spark == nullptr)
	{
		this->spark = new_AnimatedSprite3D(120, 120);
		this->spark->softAttenuation = 0;
	}
	if (this->shaftEnd == nullptr)
	{
		this->shaftEnd = new_AnimatedSprite3D(200, 200);
		this->shaftEnd->softAttenuation = 0;
		this->shaftEnd->originY = 0.65;
	}
}

void HealingGunGraphicEffect::init(IsisActionType mode,
								   HealingGunSFXData *sfxData,
								   Object3D *turret,
								   const Vector3 *localSourcePosition,
								   TankData *targetData)
{
	this->sfxData = sfxData;
	this->turret = turret;
	this->_mode = mode;
	this->targetTankData = targetData;
	this->localSourcePosition->copy(localSourcePosition);
	this->dead = false;
	this->currentFrame = 0;
	this->time = 0;
	this->shaftEndFrame = 0;
	this->updateMode();
	this->createLightEffect(mode, targetData);
}


void HealingGunGraphicEffect::createLightEffect(IsisActionType type, TankData *targetData)
{
	if (this->lightEffect != nullptr)
	{
		this->stopLightEffect();
	}

	ObjectPoolService *o = bfModel->getObjectPoolService();
	MuzzlePositionProvider *m = o->getMuzzlePositionProvider();
	this->lightEffect = o->getOmniStreamLightEffect();

	m->init(this->turret, this->localSourcePosition);

	switch (type)
	{
		case IsisActionType::DAMAGE:
			this->lightEffect->init(m, LightDataManager::getLightData(targetData->turret->getId(), "enemy_start"), LightDataManager::getLightData(targetData->turret->getId(), "enemy_loop"));
			break;
		case IsisActionType::HEAL:
			//trace("create heal effect", targetData->turret->getId(), m, LightDataManager.getLightData(targetData.turret.id, "friendly_start"), LightDataManager.getLightData(targetData->turret->getId(), "friendly_loop"));
			this->lightEffect->init(m, LightDataManager::getLightData(targetData->turret->getId(), "friendly_start"), LightDataManager::getLightData(targetData->turret->getId(), "friendly_loop"));
			break;
		default:
			this->lightEffect->init(m, LightDataManager::getLightData(targetData->turret->getId(), "idle_start"), LightDataManager::getLightData(targetData->turret->getId(), "idle_loop"));
			break;
	}

	this->stateLightEffectCreatedFor = type;

	this->bfModel->addGraphicEffect(this->lightEffect);
}

void HealingGunGraphicEffect::setMode(IsisActionType value)
{
	if (this->_mode == value)
		return;

	this->_mode = value;
	this->updateMode();
}

void HealingGunGraphicEffect::setTargetParams(Object3D *targetObject, const Vector3 *localTargetPosition, TankData *targetTankData, IsisActionType mode, bool showBeamLight)
{
	this->targetObject = targetObject;
	this->localTargetPosition->copy(localTargetPosition);
	this->createLightEffect(mode, targetTankData);

	if (showBeamLight)
		this->createBeamLight(mode, targetTankData, localTargetPosition);
}

ClientObjectUser *HealingGunGraphicEffect::getOwner()
{
	return nullptr;
}

bool HealingGunGraphicEffect::play(int millis, GameCamera *camera)
{
	if (this->dead)
		return false;

	this->createGraphics();
	this->time = this->time + millis;
	Matrix4 turretMatrix;
	turretMatrix.setMatrix(&this->turret->position, &this->turret->rotation);
	turretMatrix.transformVector(this->localSourcePosition, startPosition);
	this->spark->position.x = startPosition->x;
	this->spark->position.y = startPosition->y;
	this->spark->position.z = startPosition->z;
	int frame = int((this->time * this->frameRate));
	if (frame > this->currentFrame)
	{
		this->currentFrame = frame;
		this->spark->setFrameIndex(frame);
		if (this->_mode == IsisActionType::DAMAGE || this->_mode == IsisActionType::HEAL)
		{
			int len = this->shaftEndMaterials->length();
			this->shaftEndFrame = (this->shaftEndFrame + 1) % len;
			if (this->_mode == IsisActionType::HEAL)
			{
				this->shaftEnd->setFrameIndex(this->shaftEndFrame);
			}
			else
			{
				this->shaftEnd->setFrameIndex(int(((len - this->shaftEndFrame) - 1)));
			}
		}
	}
	if (this->_mode == IsisActionType::DAMAGE || this->_mode == IsisActionType::HEAL)
	{
		this->updateShaft(camera);
	}
	return true;
}

void HealingGunGraphicEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	this->updateMode();
}




void HealingGunGraphicEffect::destroy()
{
	if (this->shaftPlane != nullptr)
	{
		this->shaftPlane->removeFromParent();
	}
	if (this->shaftEnd != nullptr)
	{
		this->shaftEnd->removeFromParent();
	}
	if (this->spark != nullptr)
	{
		this->spark->removeFromParent();
	}
	this->container = nullptr;
	this->shaftPlane->setMaterialToAllFaces(nullptr);
	this->lightEffect->stop();

	if (this->beamEffect != nullptr)
	{
		this->beamEffect->kill();
	}

	this->lightEffect = nullptr;
	this->beamEffect = nullptr;
	this->sfxData = nullptr;
	this->sparkMaterials = nullptr;
	this->shaftMaterials = this->shaftEndMaterials = nullptr;
	this->turret = this->targetObject = nullptr;
	this->listener->onEffectDestroyed(this);
}

void HealingGunGraphicEffect::kill()
{
	this->dead = true;
}



void HealingGunGraphicEffect::updateShaft(GameCamera *camera)
{
	this->shaftPlane->setMaterialToAllFaces(getRandomFrame(this->shaftMaterials));

	Vector3 endPosition;

	Matrix4 targetMatrix;
	targetMatrix.setMatrix(&this->targetObject->position, &this->targetObject->rotation);
	targetMatrix.transformVector(this->localTargetPosition, &endPosition);

	Vector3 direction;


	direction.diff(&endPosition, startPosition);
	double beamLength = (direction.length() - endPositionOffset);

	if (beamLength < 0)
		beamLength = 10;

	this->shaftPlane->setLength(beamLength);

	direction.normalize();
	endPosition.x = (startPosition->x + (beamLength * direction.x));
	endPosition.y = (startPosition->y + (beamLength * direction.y));
	endPosition.z = (startPosition->z + (beamLength * direction.z));
	this->shaftEnd->position.x = endPosition.x;
	this->shaftEnd->position.y = endPosition.y;
	this->shaftEnd->position.z = endPosition.z;

	Vector3 cameraPosition;
	cameraPosition.x = camera->position.x;
	cameraPosition.y = camera->position.y;
	cameraPosition.z = camera->position.z;

	SFXUtils::alignObjectPlaneToView(this->shaftPlane.get(), startPosition, &direction, &cameraPosition);
}




void HealingGunGraphicEffect::createBeamLight(IsisActionType type, TankData *targetTankData, const Vector3 *localTargetPosition)
{
	(void)localTargetPosition;

	if (this->beamEffect != nullptr)
	{
		this->beamEffect->kill();
		this->beamEffect = nullptr;
	}

	if (type == IsisActionType::IDLE)
		return;


	ObjectPoolService *o = bfModel->getObjectPoolService();
	MuzzlePositionProvider *positionProvider = o->getMuzzlePositionProvider();
	MuzzlePositionProvider *targetProvider = o->getMuzzlePositionProvider();
	positionProvider->init(this->turret, this->localSourcePosition);
	targetProvider->init(this->targetObject, this->localTargetPosition);
	this->beamEffect = o->getTubeLightEffect();
	switch (type)
	{
		case IsisActionType::DAMAGE:
			this->beamEffect->init(positionProvider, targetProvider, LightDataManager::getLightData(targetTankData->turret->getId(), "enemy_beam"), true);
			break;
		case IsisActionType::HEAL:
			this->beamEffect->init(positionProvider, targetProvider, LightDataManager::getLightData(targetTankData->turret->getId(), "friendly_beam"), true);
			break;
		default:
			break;
	}
	this->bfModel->addGraphicEffect(this->beamEffect);
}

void HealingGunGraphicEffect::stopLightEffect()
{
	this->lightEffect->stop();
	this->lightEffect = nullptr;
}


void HealingGunGraphicEffect::updateMode()
{
	if (this->container == nullptr)
		return;

	switch (this->_mode)
	{
		case IsisActionType::IDLE:
			this->setIdleMode();
			break;
		case IsisActionType::HEAL:
			this->setHealMode();
			break;
		case IsisActionType::DAMAGE:
			this->setDamageMode();
	}
}

void HealingGunGraphicEffect::setIdleMode()
{
	this->createGraphics();
	this->shaftPlane->removeFromParent();
	this->shaftEnd->removeFromParent();
	if (this->spark->getParent() == nullptr)
	{
		this->container->addChild(this->spark.get());
	}
	this->sparkMaterials = this->sfxData->idleSparkData;
	this->spark->setAnimationData(this->sfxData->idleSparkData);
	this->spark->setFrameIndex(0);
}

void HealingGunGraphicEffect::setHealMode()
{
	this->createGraphics();
	if (this->shaftPlane->getParent() == nullptr)
	{
		this->container->addChild(this->shaftPlane.get());
		this->container->addChild(this->shaftEnd.get());
		this->shaftMaterials = this->sfxData->healShaftMaterials;
		std::shared_ptr<Material> material = this->shaftMaterials->at(0);
		this->shaftPlane->setMaterial(material);
		this->shaftEndMaterials = this->sfxData->healShaftEndMaterials;
		this->shaftEnd->setAnimationData(this->sfxData->healShaftEndData);
		this->shaftEnd->setFrameIndex(0);
	}
	if (this->spark->getParent() == nullptr)
	{
		this->container->addChild(this->spark.get());
	}
	this->sparkMaterials = this->sfxData->healSparkData;
	this->spark->setAnimationData(this->sfxData->healSparkData);
	this->spark->setFrameIndex(0);
}

void HealingGunGraphicEffect::setDamageMode()
{
	this->createGraphics();
	if (this->shaftPlane->getParent() == nullptr)
	{
		this->container->addChild(this->shaftPlane.get());
		this->container->addChild(this->shaftEnd.get());
		this->shaftMaterials = this->sfxData->damageShaftMaterials;
		std::shared_ptr<Material> m = this->shaftMaterials->at(0);
		this->shaftPlane->setMaterial(m);
		this->shaftEndMaterials = this->sfxData->damageShaftEndMaterials;
		this->shaftEnd->setAnimationData(this->sfxData->damageShaftEndData);
		this->shaftEnd->setFrameIndex(0);
	}
	if (this->spark->getParent() == nullptr)
	{
		this->container->addChild(this->spark.get());
	}
	this->sparkMaterials = this->sfxData->damageSparkData;
	this->spark->setAnimationData(this->sfxData->damageSparkData);
	this->spark->setFrameIndex(0);
}
