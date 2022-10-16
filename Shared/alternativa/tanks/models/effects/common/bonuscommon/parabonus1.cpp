#include "parabonus1.h"
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
#include "parachute1.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidableobject3dwrapper.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidablegraphicobjects.h"
#include "__global.h"
#include "cords1.h"
#include <QDebug>


const Body::T *ParaBonus1::TYPE = new Body::T("ParaBonus");



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



ParaBonus1::ParaBonus1(
		const T *type,
		ParaBonusPool *pool,
		std::shared_ptr<Mesh> &boxMesh,
		std::shared_ptr<Mesh> &parachuteMesh,
		std::shared_ptr<Mesh> &parachuteInnerMesh,
		Material *cordMaterial)
	: SkinnedBody3D(type, 1. / BOX_MASS, &Matrix3::IDENTITY)
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

	this->bonusState = BonusState::FALLING;

	this->hidabbleWrapper = nullptr;

	this->parachute = nullptr;
	const Vector3 hs(BOX_HALF_SIZE, BOX_HALF_SIZE, BOX_HALF_SIZE);
	PhysicsUtils::getBoxInvInertia(BOX_MASS, &hs, &invInertia);
	collisionBox = new CollisionBox(&hs, CollisionGroup::BONUS_WITH_STATIC);

	collisionBox->postCollisionPredicate = this;
	addCollisionPrimitive(collisionBox);

	canFreeze = true;

	this->skin = Mesh_clone(boxMesh);
#ifdef GRAPHICS
	this->hidabbleWrapper = new HidableObject3DWrapper(this->skin.get());
#endif
	this->createParachuteAndCords(parachuteMesh, parachuteInnerMesh, cordMaterial);
}

ParaBonus1::~ParaBonus1()
{
	//delete collisionBox;

#ifdef GRAPHICS
	if (this->hidabbleWrapper != nullptr)
		delete this->hidabbleWrapper;
#endif

	parachute = nullptr;
	_boxMesh = nullptr;
}

ParaBonus1* ParaBonus1::create(BonusCommonData *bonusData)
{
	std::shared_ptr<Mesh> parachuteInnerMesh;
	return new ParaBonus1(ParaBonus1::TYPE, nullptr, bonusData->boxMesh, bonusData->parachuteMesh, parachuteInnerMesh, bonusData->cordMaterial);
}


CollisionBox* ParaBonus1::getCollisionBox()
{
	return collisionBox;
}

Mesh* ParaBonus1::getMesh()
{
	return skin.get();
}

bool ParaBonus1::isFlatSurface(Vector3 *groundNormal)
{
	return groundNormal->z > BonusConst::COS_ONE_DEGREE;
}

void ParaBonus1::init(const QString &bonusId, int timeToLive, bool isFalling)
{
	this->_bonusId = bonusId;
	this->timeToLive = (timeToLive < 0) ? INT_MAX : timeToLive;
	this->bonusState = isFalling ? BonusState::FALLING : BonusState::RESTING;

	this->additiveValue = 0;
	this->visibilitySwitchTime = 0;
	this->cordsMesh->alpha = 1;
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


	collisionPrimitives->head->primitive->collisionGroup |= CollisionGroup::BONUS_WITH_TANK;

	this->parachute->outerMesh->alpha = 1;
	this->frozen = false;
	this->freezeCounter = 0;

	BodyState defaultState;
	defaultState.pos.reset();
	defaultState.orientation.reset();
	defaultState.velocity.reset();
	defaultState.rotation.reset();

	state.copy(&defaultState);
	prevState.copy(&defaultState);
	this->parachute->state.copy(&defaultState);
	this->parachute->prevState.copy(&defaultState);

	this->skin->alpha = 1.;
}

QString ParaBonus1::getBonusId()
{
	return this->_bonusId;
}

bool ParaBonus1::isFalling()
{
	return this->bonusState == BonusState::FALLING;
}

void ParaBonus1::readBonusPosition(Vector3 *result)
{
	result->copy(&this->skin->position);
}

/*
void ParaBonus::setBonusPosition(double x, double y, double z)
{
	this->skin->position.reset(x, y, z);
}

void ParaBonus::setRestingState(double x, double y, double z)
{
	this->setBonusPosition(x, y, z);
	if (this->bonusState != BonusState::RESTING)
	{
		this->bonusState = BonusState::RESTING;
		//this->detachParachute();
	}
}
*/
void ParaBonus1::setTakenState()
{
	this->takenAnimationTime = TAKEN_ANIMATION_TIME;
	this->skin->alpha = 1;
	if (this->bonusState == BonusState::FALLING)
		this->detachParachute();

	this->bonusState = BonusState::TAKEN;
}

void ParaBonus1::setRemovedState()
{
	this->bonusState = BonusState::REMOVING;
}

void ParaBonus1::attach(const Vector3 *pos, PhysicsScene *rigidWorld, Scene3DContainer *container, IBonusListener *listener)
{
	rigidWorld->addBody(this);

	TanksCollisionDetector *collisionDetector = (TanksCollisionDetector*)rigidWorld->collisionDetector;
	collisionDetector->addBody(this);

	if (container != nullptr)
	{
		//if (this->battleField.gameMode != GameModes.DAY)
			//container.addChild(this->light);

		container->addChild(this->skin.get());
		//BattlefieldModel(Main.osgi.getService(IBattleField)).hidableObjects.add(this->hidabbleWrapper);
	}
	state.pos.x = pos->x + 50;
	state.pos.y = pos->y + 50;
	state.pos.z = pos->z;
	state.rotation.z = 0.15;

	//if (this->battleField.gameMode != GameModes.DAY)
		//this->light.x = state.pos.x;

	//this->light.y = state.pos.y;
	//this->light.z = state.pos.z;

	prevState.copy(&state);
	if (this->bonusState == BonusState::FALLING)
	{
		rigidWorld->addBody(this->parachute.get());
		this->parachute->addToContainer(container);

		for (int i = 0; i < 4; i++)
			rigidWorld->addConstraint(this->cordsConstaints[i].get());

		this->parachute->state.pos.copy(pos);
		this->parachute->state.pos.z = (this->parachute->state.pos.z + (0.5 * CORDS_LENGTH));
		this->parachute->state.rotation.z = 0.3;
		this->parachute->prevState.copy(&this->parachute->state);
		if (container != nullptr)
		{
			container->addChild(this->cordsMesh.get());
		}
		this->physicsState = PHYSICS_STATE::FULL;
	}
	else
	{
		this->physicsState = PHYSICS_STATE::BOX;
	}
	this->updateSkin(1);
	this->skin->useLight = false;
	this->bonusListener = listener;
}

void ParaBonus1::beforePhysicsStep(double dt)
{
	(void)dt;
}


bool ParaBonus1::update(int time, int millis, double interpolationTime)
{
	this->timeToLive -= millis;

	this->updateSkin(interpolationTime);

	if (this->bonusState == BonusState::FALLING)
	{
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


	if (this->parachuteTimeLeft > 0)
	{
		this->parachuteTimeLeft = this->parachuteTimeLeft - millis;
		if (this->parachuteTimeLeft <= 0)
		{
			this->removeParachuteGraphics();
			this->removeParachutePhysics();
		}
		else
		{
			this->parachute->outerMesh->alpha = double(this->parachuteTimeLeft) / PARACHUTE_REMOVAL_TIME;
		}
	}


	if ((this->bonusState == BonusState::RESTING || (this->bonusState == BonusState::REMOVING)) && (this->timeToLive < WARNING_TIME))
	{
		this->playWarningAnimation(time, millis);
	}

	if (this->bonusState == BonusState::REMOVING && this->timeToLive > WARNING_TIME)
	{
		return false;
	}
	return this->bonusState != BonusState::REMOVED;
}

void ParaBonus1::destroy()
{
	skin->removeFromParent();
	//if (this->battleField.gameMode != GameModes.DAY)
	{
		//_local_1 = this.light;
		//(_local_1.alternativa3d::removeFromParent());
	}
	//BattlefieldModel(Main.osgi.getService(IBattleField)).hidableObjects.remove(this.hidabbleWrapper);
	if (skin != nullptr)
	{
		skin->destroy();
		skin = nullptr;
	}
	/*
	if (_skin != nullptr)
	{
		_skin.destroy();
		_skin = nullptr;
	}
	*/
	this->removeParachuteGraphics();
	this->removeCordConstraints();
	this->removeParachutePhysics();
	TanksCollisionDetector *collisionDetector = (TanksCollisionDetector*)world->collisionDetector;
	collisionDetector->removeBody(this);

	world->removeBody(this);
	//_local_1 = this.pool.numObjects++;
	//this.pool.objects[_local_1] = this;

	/*
	physicsScene->removeBody(this);
	battlefieldModel->bfData->collisionDetector->removeBody(this);

	for (int i = 0; i < 4; i++) {
		MaxDistanceConstraint *c = this->cordsConstaints[i].get();
		if (c != nullptr) {
			if (c->world != nullptr)
				c->world->removeConstraint(c);
			this->cordsConstaints[i] = nullptr;
		}
	}

	if (this->parachute != nullptr)
	{
		if (this->parachute->outerMesh != nullptr)
		{
			this->parachute->outerMesh->destroy();
			this->parachute->outerMesh = nullptr;
		}
		physicsScene->removeBody(this->parachute.get());
		this->parachute = nullptr;
	}

	this->skin->destroy();
	this->skin = nullptr;

	//_local_1 = this->light;
	//_local_1->alternativa3d::removeFromParent();
#ifdef GRAPHICS
	this->battlefieldModel->getHidableGraphicObjects()->remove(this->hidabbleWrapper);
#endif
	*/

	delete this;
}

bool ParaBonus1::considerCollision(CollisionPrimitive *primitive)
{
	if (primitive->body == nullptr)
	{
		if (this->bonusState == BonusState::FALLING)
		{
			this->onStaticCollision();
		}
		return true;
	}

	if (primitive->body->isType(Tank::TYPE))
		this->onTankCollision();

	return false;
}

void ParaBonus1::onTriggerActivated()
{
	this->onTankCollision();
	this->setTakenState();
}

void ParaBonus1::onStaticCollision()
{
	this->bonusState = BonusState::RESTING;
	this->detachParachute();
	if (this->bonusListener != nullptr)
		this->bonusListener->onBonusDropped(this);
}


void ParaBonus1::detachParachute()
{
	this->removeCordConstraints();
	this->startParachuteDissolving();
}

void ParaBonus1::startParachuteDissolving()
{
	this->parachuteTimeLeft = PARACHUTE_REMOVAL_TIME;
}


void ParaBonus1::onTankCollision()
{
	collisionPrimitives->head->primitive->collisionGroup &= ~CollisionGroup::BONUS_WITH_TANK;
	this->bonusListener->onTankCollision(this);
}


void ParaBonus1::createParachuteAndCords(std::shared_ptr<Mesh> &parachuteMesh, std::shared_ptr<Mesh> &parachuteInnerMesh, Material *cordsMaterial)
{
	this->parachute = std::make_shared<Parachute1>(Parachute1::TYPE, PARACHUTE_MASS, PARACHUTE_RADIUS, 10, 0.8, 0, parachuteMesh, parachuteInnerMesh);
	this->cordsMesh = new_Cords1(266, BOX_HALF_SIZE, 12, skin.get(), this->parachute->outerMesh.get());
	this->cordsMesh->setMaterialToAllFaces(cordsMaterial);

	const int numStraps = 4;
	double angleStep = (2 * MyMath::PI) / numStraps;
	for (int i = 0; i < numStraps; i++)
	{
		double angle = i * angleStep;
		double x = PARACHUTE_RADIUS * MyMath::cos(angle);
		double y = PARACHUTE_RADIUS * MyMath::sin(angle);
		Vector3 r1(x, y, 0);
		Vector3 r2(0, 0, 50);
		this->cordsConstaints[i] = std::make_shared<MaxDistanceConstraint>(this->parachute.get(), this, &r1, &r2, CORDS_LENGTH);
	}
}


void ParaBonus1::removeParachuteGraphics()
{
	if (this->parachute != nullptr && this->parachute->outerMesh != nullptr)
	{
		this->parachute->removeFromContainer();
		this->parachute->outerMesh->destroy();
		this->parachute->outerMesh = nullptr;
	}
	if (this->cordsMesh != nullptr)
	{
		this->cordsMesh->removeFromParent();
		this->cordsMesh->destroy();
		this->cordsMesh = nullptr;
	}
}

void ParaBonus1::removeParachutePhysics()
{
	if (this->physicsState == PHYSICS_STATE::PARABOX)
	{
		if (this->parachute != nullptr)
		{
			physicsScene->removeBody(this->parachute.get());
			this->parachute = nullptr;
		}
		this->physicsState = PHYSICS_STATE::BOX;
	}
}


void ParaBonus1::playWarningAnimation(int time, double deltaMsec)
{
	if (this->skin == nullptr)
		return;

	if (this->visibilitySwitchTime == 0)
	{
		this->alphaSpeed = ((-(COEFF) * DELTA_ALPHA) / this->currBlinkInterval);
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

void ParaBonus1::playTakenAnimation(int millis)
{

	const double d1 = ((UP_SPEED * this->takenAnimationTime) / TAKEN_ANIMATION_TIME) + (UP_SPEED * 0.1);
	const double d2 = ((ANGLE_SPEED * this->takenAnimationTime) / TAKEN_ANIMATION_TIME) + (ANGLE_SPEED * 0.1);

	Vector3 v;
	v.x = v.y = 0.;
	v.z = world->_gravity.length() / invMass;

	addForce(&v);

	state.velocity.z = d1;
	state.rotation.z = d2;

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

void ParaBonus1::updateSkin(double t)
{
	SkinnedBody3D::updateSkin(t);


	if (this->parachute != nullptr) {
		this->parachute->updateSkin(t);
		if (this->cordsMesh != nullptr)
			this->cordsMesh->updateVertices();
	}

	//if (this->battleField.gameMode == GameModes.HALLOWEEN)
	//this->light.x = state.pos.x;

	//this->light.y = state.pos.y;
	//this->light.z = state.pos.z;
}


void ParaBonus1::removeCordConstraints()
{
	if (this->physicsState == PHYSICS_STATE::FULL)
	{
		for (int i = 0; i < 4; i++) {
			MaxDistanceConstraint *c = this->cordsConstaints[i].get();
			if (c != nullptr) {
				if (c->world != nullptr)
					c->world->removeConstraint(c);
				this->cordsConstaints[i] = nullptr;
			}
		}

		this->parachute->state.rotation.scale(0.05);
		this->parachute->saveState();
		this->physicsState = PHYSICS_STATE::PARABOX;
	}
}
