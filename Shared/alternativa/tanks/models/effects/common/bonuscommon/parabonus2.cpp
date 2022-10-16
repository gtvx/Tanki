#include "parabonus2.h"
#include <QVector>
#include <QHash>
#include "bonusconst.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/physics/physicsutils.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "BonusCommonData.h"
#include "service.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "bonustrigger.h"
#include "fallcontroller.h"
#include "landingcontroller.h"
#include "bonusmesh.h"
#include "mymath.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/physics/collisionprimitivelist.h"
#include "alternativa/physics/collisionprimitivelistitem.h"
#include "alternativa/physics/constraints/maxdistanceconstraint.h"
#include "parachute2.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidableobject3dwrapper.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidablegraphicobjects.h"
#include "__global.h"
#include "cords2.h"
#include "bonuscache.h"
#include <QDebug>


static const double BOX_HALF_SIZE = 75;
static const double BOX_MASS = 20;
static const double TAKEN_ANIMATION_TIME = 2000;
static const int FLASH_DURATION = 300;
static const int ALPHA_DURATION = TAKEN_ANIMATION_TIME - FLASH_DURATION;//1700
static const int PARACHUTE_REMOVAL_TIME = 2000;
static const int BLINK_INTERVAL = 500;
static const double BIG_VALUE = 10000000000;
static const int WARNING_TIME = 8000;
static const double COEFF = 10;
static const double DELTA_ALPHA = 0.5;
static const double MIN_ALPHA = 1. - DELTA_ALPHA; //0.5
static const double UP_SPEED = 300;
static const double ANGLE_SPEED = 2;
static const int MAX_ADDITIVE_VALUE = 204;
static const double ADDITIVE_SPEED_UP = double(MAX_ADDITIVE_VALUE) / double(FLASH_DURATION);
static const double ADDITIVE_SPEED_DOWN = double(MAX_ADDITIVE_VALUE) / (double(TAKEN_ANIMATION_TIME) - double(FLASH_DURATION));
static const double PARACHUTE_RADIUS = 180;
static const double PARACHUTE_MASS = 10;
static const double CORDS_LENGTH = 400;


ParaBonus2::ParaBonus2(
		ParaBonusPool *pool,
		std::shared_ptr<Mesh> &boxMesh,
		std::shared_ptr<Mesh> &parachuteMesh,
		std::shared_ptr<Mesh> &parachuteInnerMesh,
		Material *cordMaterial)
#if __BONUS_TYPE == 1
	: SkinnedBody3D(type, 1. / BOX_MASS, &Matrix3::IDENTITY)
	#endif
{
	(void)pool;

	battlefieldModel = Service::getBattlefieldModel();

	if (battlefieldModel == nullptr)
	{
		return;
	}

	physicsScene = battlefieldModel->bfData->physicsScene;

	physicsState = PHYSICS_STATE::NONE;
	additiveValue = 0;
	parachuteTimeLeft = 0;
	takenAnimationTime = 0;
	visibilitySwitchTime = 0;
	currBlinkInterval = 0;
	timeToLive = 0;
	alphaSpeed = 0;


	this->parachute = nullptr;
	this->bonusState = BonusState::FALLING;

	this->hidabbleWrapper = nullptr;

	fallSpeed = 200;
	landingControllerInited = false;
	fallPrecision = 3;
	landingPrecision = 3;

	this->_boxMesh = boxMesh;
	this->_parachuteMesh = parachuteMesh;
	this->_parachuteInnerMesh = parachuteInnerMesh;
	this->_cordMaterial = cordMaterial;

	this->fallController = new FallController(this);
	this->landingController = new LandingController(this);
	this->trigger = std::make_shared<BonusTrigger>(this);
}

ParaBonus2::~ParaBonus2()
{
	if (this->fallController != nullptr)
		delete this->fallController;
	if (this->landingController != nullptr)
		delete this->landingController;

#ifdef GRAPHICS
	//if (this->hidabbleWrapper != nullptr)
	//delete this->hidabbleWrapper;
#endif

	_boxMesh = nullptr;
}

ParaBonus2* ParaBonus2::create(BonusCommonData *bonusData)
{
	std::shared_ptr<Mesh> parachuteInnerMesh;
	ParaBonus2 *r = new ParaBonus2(nullptr, bonusData->boxMesh, bonusData->parachuteMesh, parachuteInnerMesh, bonusData->cordMaterial);
	return r;
}


CollisionBox *ParaBonus2::getCollisionBox()
{
	return trigger->collisionBox;
}

Mesh* ParaBonus2::getMesh()
{
	return skin.get();
}

bool ParaBonus2::isFlatSurface(Vector3 *groundNormal)
{
	return groundNormal->z > BonusConst::COS_ONE_DEGREE;
}

void ParaBonus2::init(const QString &bonusId, int timeToLive, bool isFalling)
{
	if (BonusCache::isBonusMeshCacheEmpty(this->_bonusId))
	{
		this->skin = new_BonusMesh(this->_bonusId, this->_boxMesh.get());
	}
	else
	{
		this->skin = BonusCache::getBonusMesh(this->_bonusId);
	}

#ifdef GRAPHICS
	//this->light = new OmniLight(colorLight, 150, 500);
	this->hidabbleWrapper = new HidableObject3DWrapper(this->skin.get());
#endif
	this->createParachuteAndCords(this->_parachuteMesh, this->_parachuteInnerMesh, this->_cordMaterial);
	this->cordsMesh->alpha = 1;


	this->_bonusId = bonusId;
	this->timeToLive = (timeToLive < 0) ? INT_MAX : timeToLive;
	this->bonusState = isFalling ? BonusState::FALLING : BonusState::RESTING;


	this->additiveValue = 0;
	this->visibilitySwitchTime = 0;
	this->parachuteTimeLeft = PARACHUTE_REMOVAL_TIME;
	this->takenAnimationTime = TAKEN_ANIMATION_TIME;
	this->currBlinkInterval = BLINK_INTERVAL;

	//var colorTransform:ColorTransform = skin.colorTransform;
	//if (colorTransform != null)
	//{
	//colorTransform.redOffset = 0;
	//colorTransform.greenOffset = 0;
	//colorTransform.blueOffset = 0;
	//}




	/*
	uint colorLight = 16745512;
	if (bonusId.indexOf("health") >= 0)
	{
		colorLight = 0xFF00;
	}

	if (bonusId.indexOf("armor") >= 0)
	{
		colorLight = 0xFFFFFF;
	}

	if (bonusId.indexOf("damage") >= 0)
	{
		colorLight = 0xFFFF;
	}

	if (bonusId.indexOf("nitro") >= 0)
	{
		colorLight = 0xFF0000;
	}

	if (bonusId.indexOf("crystall") >= 0)
	{
		colorLight = 8454143;
	}

	if (bonusId.indexOf("gold") >= 0)
	{
		colorLight = 16777088;
	}
	*/

	this->skin->alpha = 1.;
}

void ParaBonus2::getGroundPointAndNormal(const Vector3 *spawnPosition, Vector3 *point, Vector3 *normal)
{
	RayIntersection rayHit;

	TanksCollisionDetector *collisionDetector = battlefieldModel->bfData->collisionDetector;
	if (collisionDetector->intersectRayWithStatic(spawnPosition,
												  &Vector3::DOWN,
												  CollisionGroup::STATIC,
												  BIG_VALUE,
												  nullptr,
												  &rayHit))
	{
		normal->copy(&rayHit.normal);
		point->copy(&rayHit.pos);
	}
	else
	{
		normal->copy(&Vector3::Z_AXIS);
		point->copy(spawnPosition);
		point->z = point->z - 1000.;
	}
}


QString ParaBonus2::getBonusId()
{
	return this->_bonusId;
}

bool ParaBonus2::isFalling()
{
	return this->bonusState == BonusState::FALLING;
}

void ParaBonus2::readBonusPosition(Vector3 *result)
{
	result->copy(&this->skin->position);
}

/*
void ParaBonus2::setBonusPosition(double x, double y, double z)
{
	this->skin->position.reset(x, y, z);
}

void ParaBonus2::setRestingState(double x, double y, double z)
{
	this->setBonusPosition(x, y, z);
	if (this->bonusState != BonusState::RESTING)
	{
		this->bonusState = BonusState::RESTING;
		//this->detachParachute();
	}
}
*/
void ParaBonus2::setTakenState()
{
	this->takenAnimationTime = TAKEN_ANIMATION_TIME;
	this->skin->setAlpha(1);
	if (this->bonusState == BonusState::FALLING)
		this->detachParachute();

	this->bonusState = BonusState::TAKEN;
}

void ParaBonus2::setRemovedState()
{
	this->bonusState = BonusState::REMOVING;
}

void ParaBonus2::attach(const Vector3 *pos, PhysicsScene *rigidWorld, Scene3DContainer *container, IBonusListener *listener)
{
	(void)rigidWorld;

	if (container != nullptr)
	{
#ifdef GRAPHICS
		//container->addChild(this->light);
		container->addChild(this->skin.get());
		this->battlefieldModel->getHidableGraphicObjects()->add(this->hidabbleWrapper);
#endif
	}

	if (this->bonusState == BonusState::FALLING)
	{
		this->parachute->position.x = pos->x;
		this->parachute->position.y = pos->y;
		this->parachute->position.z = pos->z + (0.5 * CORDS_LENGTH);
#ifdef GRAPHICS
		container->addChild(this->parachute.get());
		container->addChild(this->cordsMesh.get());
#endif
		this->physicsState = PHYSICS_STATE::FULL;
	}
	else
	{
		this->physicsState = PHYSICS_STATE::BOX;
	}


	this->bonusListener = listener;

	Vector3 P, N, X, Y, Y1, P1, origin;

	this->getGroundPointAndNormal(pos, &P, &N);


	if (this->bonusState == BonusState::FALLING)
	{
		double fallTime;

		if (isFlatSurface(&N))
		{
			fallTime = this->calculateFallTime(pos, &P);
			P1.copy(&P);
		}
		else
		{
			X.cross2(&N, &Vector3::Z_AXIS);
			X.normalize();
			Y.cross2(&N, &X);
			Y1.cross2(&Vector3::Z_AXIS, &X);
			origin.copy(pos);
			origin.addScaled(-BonusConst::BONUS_HALF_SIZE, &Y1);
			P1.copy(&P);
			P1.addScaled(-BonusConst::BONUS_HALF_SIZE / N.z, &Y);

			RayIntersection rayHit;

			if (battlefieldModel->bfData->collisionDetector->intersectRayWithStatic(&origin,
																					&Vector3::DOWN,
																					CollisionGroup::STATIC,
																					BIG_VALUE,
																					nullptr,
																					&rayHit))
			{
				if (((P.z < rayHit.pos.z) && (rayHit.pos.z < P1.z)))
				{
					P1.addScaled((((BonusConst::BONUS_HALF_SIZE / N.z) * (P1.z - rayHit.pos.z)) / (P1.z - P.z)), &Y);
				}
			}
			fallTime = this->calculateFallTime(pos, &P1);
			this->landingController->init(&P1, &N);
			this->landingControllerInited = true;
		}

		double startingAngleZ = this->getStartingAngleZ();
		this->skin->position.copy(pos);
		this->skin->rotation.reset(0, 0, startingAngleZ);

		this->trigger->update(this->skin->position.x, this->skin->position.y, this->skin->position.z,
							  this->skin->rotation.x, this->skin->rotation.y, this->skin->rotation.z);

		double minPivotZ = ((P1.z + BonusConst::BONUS_HALF_SIZE) + BonusConst::BONUS_OFFSET_Z);
		this->fallController->init(pos, this->fallSpeed, minPivotZ, -fallTime, 0, startingAngleZ);
	}
	this->trigger->activate(battlefieldModel);
}

void ParaBonus2::beforePhysicsStep(double dt)
{
	(void)dt;
}

void ParaBonus2::onLandingComplete()
{
	this->landingControllerInited = false;
}

double ParaBonus2::getStartingAngleZ()
{
	return ((MyMath::PI * 10.) * QString(this->_bonusId.split("_")[1]).toInt()) / 180.;
}

double ParaBonus2::calculateFallTime(const Vector3 *spawnPosition, const Vector3 *groundTouchPoint)
{
	return ((spawnPosition->z - groundTouchPoint->z) - BonusConst::BONUS_HALF_SIZE) / this->fallSpeed;
}


bool ParaBonus2::update(int time, int millis, double interpolationTime)
{
	this->timeToLive -= millis;

	if (this->bonusState == BonusState::FALLING)
	{
		for (int i = 0; i < this->fallPrecision; i++)
		{
			this->fallController->runBeforePhysicsUpdate(millis / this->fallPrecision);
			this->fallController->interpolatePhysicsState(interpolationTime);
			this->fallController->render();
		}
		return true;
	}


	if (this->bonusState == BonusState::TAKEN)
	{
		if (this->takenAnimationTime < 0)
		{
			return false;
		}
		this->playTakenAnimation(millis);
	}


	if (((this->bonusState == BonusState::RESTING) && (this->landingControllerInited)))
	{
		for (int i = 0; i < this->landingPrecision; i++)
		{
			this->landingController->runBeforePhysicsUpdate(millis / this->landingPrecision);
			this->landingController->interpolatePhysicsState(interpolationTime);
			this->landingController->render();
		}
	}


	if (this->parachuteTimeLeft > 0)
	{
		this->parachuteTimeLeft = (this->parachuteTimeLeft - millis);
		if (this->parachuteTimeLeft <= 0)
		{
			this->removeParachuteGraphics();
			this->removeParachutePhysics();
		}
		else
		{
			this->parachute->position.z -= (this->fallSpeed / 2000) * millis;
			double v = double(this->parachuteTimeLeft) / PARACHUTE_REMOVAL_TIME;
			this->cordsMesh->setAlpha(v);
			this->parachute->setAlpha(v);
			this->cordsMesh->updateVertices();
		}
	}



	if (((this->bonusState == BonusState::RESTING) || (this->bonusState == BonusState::REMOVING)) && (this->timeToLive < WARNING_TIME))
	{
		this->playWarningAnimation(time, millis);
	}

	if ((this->bonusState == BonusState::REMOVING) && (this->timeToLive > WARNING_TIME))
	{
		return false;
	}

	return this->bonusState != BonusState::REMOVED;
}

void ParaBonus2::destroy()
{
	this->skin->removeFromParent();
	this->skin->recycle();
	//this->skin->destroy();
	this->skin = nullptr;

	this->removeParachuteGraphics();
	this->removeParachutePhysics();


	//_local_1 = this->light;
	//_local_1->alternativa3d::removeFromParent();
#ifdef GRAPHICS
	this->battlefieldModel->getHidableGraphicObjects()->remove(this->hidabbleWrapper);
#endif

	this->trigger->deactivate();
	this->trigger = nullptr;

	delete this;
}

bool ParaBonus2::considerCollision(CollisionPrimitive *primitive)
{
	if (primitive->body->isType(Tank::TYPE))
		this->onTankCollision();

	return false;
}

void ParaBonus2::onTriggerActivated()
{
	this->onTankCollision();
	this->setTakenState();
	this->trigger->deactivate();
}

void ParaBonus2::onStaticCollision()
{
	this->bonusState = BonusState::RESTING;
	this->detachParachute();
	if (this->bonusListener != nullptr)
		this->bonusListener->onBonusDropped(this);

	if (this->landingControllerInited)
		this->landingController->start();
}


void ParaBonus2::detachParachute()
{
	this->startParachuteDissolving();
}

void ParaBonus2::startParachuteDissolving()
{
	this->parachuteTimeLeft = PARACHUTE_REMOVAL_TIME;
}


void ParaBonus2::onTankCollision()
{
	this->bonusListener->onTankCollision(this);
}


void ParaBonus2::createParachuteAndCords(std::shared_ptr<Mesh> &parachuteMesh, std::shared_ptr<Mesh> &parachuteInnerMesh, Material *cordsMaterial)
{
	if (BonusCache::isParachuteCacheEmpty())
		this->parachute = new_Parachute2(parachuteMesh.get(), parachuteInnerMesh.get());
	else
		this->parachute = BonusCache::getParachute();

	if (BonusCache::isCordsCacheEmpty())
		this->cordsMesh = new_Cords2(Parachute2::RADIUS, BOX_HALF_SIZE, Parachute2::NUM_STRAPS, cordsMaterial);
	else
		this->cordsMesh = BonusCache::getCords();

	this->cordsMesh->init(this->skin.get(), this->parachute.get());
}


void ParaBonus2::removeParachuteGraphics()
{
	if (this->parachute != nullptr)
	{
		this->parachute->removeFromParent();
		this->parachute->recycle();
		this->parachute = nullptr;
	}
	if (this->cordsMesh != nullptr)
	{
		this->cordsMesh->removeFromParent();
		this->cordsMesh->recycle();
		this->cordsMesh = nullptr;
	}
}

void ParaBonus2::removeParachutePhysics()
{
	if (this->physicsState == PHYSICS_STATE::PARABOX)
	{
		if (this->parachute != nullptr)
		{
			this->parachute = nullptr;
		}
		this->physicsState = PHYSICS_STATE::BOX;
	}
}


void ParaBonus2::playWarningAnimation(int time, double deltaMsec)
{
	if (this->skin == nullptr)
		return;

	if (this->visibilitySwitchTime == 0)
	{
		this->alphaSpeed = (-COEFF * DELTA_ALPHA) / this->currBlinkInterval;
		this->visibilitySwitchTime = (time + this->currBlinkInterval);
	}
	else
	{
		this->skin->alpha = this->skin->alpha + (this->alphaSpeed * deltaMsec);
		if ((this->bonusState == BonusState::REMOVING) && (this->alphaSpeed < 0))
		{
			if (this->skin->alpha <= 0)
			{
				this->bonusState = BonusState::REMOVED;
			}
		}
		else
		{
			if (this->skin->alpha < MIN_ALPHA)
			{
				this->skin->alpha = MIN_ALPHA;
			}

			if (time >= this->visibilitySwitchTime)
			{
				if (this->currBlinkInterval > 22)
					this->currBlinkInterval = this->currBlinkInterval - 12;

				this->visibilitySwitchTime = (this->visibilitySwitchTime + this->currBlinkInterval);
				if (this->alphaSpeed < 0)
				{
					this->alphaSpeed = (COEFF * DELTA_ALPHA) / this->currBlinkInterval;
					this->skin->alpha = MIN_ALPHA;
				}
				else
				{
					this->alphaSpeed = (-(COEFF) * DELTA_ALPHA) / this->currBlinkInterval;
					this->skin->alpha = 1;
				}
			}
		}
	}

	if (this->currBlinkInterval < 22)
	{
		this->skin->alpha = 0;
	}
}

void ParaBonus2::playTakenAnimation(int millis)
{

	const double d1 = ((UP_SPEED * this->takenAnimationTime) / TAKEN_ANIMATION_TIME) + (UP_SPEED * 0.1);
	const double d2 = ((ANGLE_SPEED * this->takenAnimationTime) / TAKEN_ANIMATION_TIME) + (ANGLE_SPEED * 0.1);

	double dt = millis * 0.001;
	this->skin->position.z += d1 * dt;
	this->skin->rotation.z += d2 * dt;


	if (this->takenAnimationTime > (TAKEN_ANIMATION_TIME - FLASH_DURATION))
	{
		this->additiveValue = this->additiveValue + (ADDITIVE_SPEED_UP * millis);
		if (this->additiveValue > MAX_ADDITIVE_VALUE)
		{
			this->additiveValue = MAX_ADDITIVE_VALUE;
		}
	}
	else
	{
		this->additiveValue = this->additiveValue - (ADDITIVE_SPEED_DOWN * millis);
		if (this->additiveValue < 0)
		{
			this->additiveValue = 0;
		}
	}


	//var colorTransform:ColorTransform = skin.colorTransform;
	//if (colorTransform == null)
	//{
	//colorTransform = new ColorTransform();
	//skin.colorTransform = colorTransform;
	//}

	//colorTransform.redOffset = this->additiveValue;
	//colorTransform.blueOffset = this->additiveValue;
	//colorTransform.greenOffset = this->additiveValue;


	if (this->takenAnimationTime < ALPHA_DURATION)
	{
		this->skin->alpha = this->takenAnimationTime / double(ALPHA_DURATION);
	}

	this->takenAnimationTime = this->takenAnimationTime - millis;
}

