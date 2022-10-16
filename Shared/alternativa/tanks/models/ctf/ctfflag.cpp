#include "ctfflag.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/vehicles/tanks/tankskinturret.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/engine3d/animatedsprite3d.h"
#include "alternativa/tanks/utils/graphicsutils.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidablegraphicobjects.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidableobject3dwrapper.h"
#include "_global.h"
#include "service.h"
#include <QString>


#define SKIN_HEIGHT 400
#define SKIN_BASE_SIZE 100

const double CTFFlag::Z_DISPLACEMENT = 50;
static const Vector3 rayDirection(0, 0, -20000);



CTFFlag::CTFFlag(BattleTeamType teamType, Vector3 *basePos, int frameWidth, int frameHeight, void *materials, std::shared_ptr<BitmapData> &originalTexture, TanksCollisionDetector *collisionDetector)
{
	_function_name("CTFFlag::CTFFlag");
	this->triggerCollisionPrimitive = nullptr;
	this->_carrierId = new QString();
	this->takeCommandSent = false;
	this->_state = CTFFlagState::AT_BASE;
	this->teamType = teamType;
	this->_basePosition.copy(basePos);
	this->originalTexture = originalTexture;
	this->createSkin(frameWidth, frameHeight, materials);
	this->_basePosition.z += 50;
	RayIntersection intersection;
	if (collisionDetector->intersectRayWithStatic(&this->_basePosition, &rayDirection, CollisionGroup::STATIC, 1, nullptr, &intersection))
		this->_basePosition.copy(&intersection.pos);
	this->_basePosition.z +=  + Z_DISPLACEMENT;
	this->setPosition(&this->_basePosition);
	this->hidabbleWrapper = nullptr;

	//int frameInterval = 1000;
	//this->startTime = this->frameInterval * Math.random();
}

CTFFlag::~CTFFlag()
{
	_function_name("CTFFlag::~CTFFlag");
	delete this->_carrierId;
	if (triggerCollisionPrimitive != nullptr)
		delete triggerCollisionPrimitive;
#ifdef GRAPHICS
	if (this->hidabbleWrapper != nullptr)
		delete this->hidabbleWrapper;
#endif
}

QString CTFFlag::getCarrierId()
{
	_function_name("CTFFlag::getCarrierId");
	return *this->_carrierId;
}

void CTFFlag::setPosition(Vector3 *value)
{
	_function_name("CTFFlag::setPosition");
	this->skin->position.copy(value);
	this->triggerCollisionPrimitive->transform.d = value->x;
	this->triggerCollisionPrimitive->transform.h = value->y;
	this->triggerCollisionPrimitive->transform.l = value->z + (0.5 * SKIN_HEIGHT);
	this->triggerCollisionPrimitive->calculateAABB();
}

void CTFFlag::addToContainer(Scene3DContainer *container)
{
	_function_name("CTFFlag::addToContainer");
	this->container = container;
	this->container->addChild(this->skin.get());
#ifdef GRAPHICS
	Service::getBattlefieldModel()->getHidableGraphicObjects()->add(hidabbleWrapper);
#endif
}

void CTFFlag::setCarrier(const QString &carrierId, TankData *carrierData)
{
	_function_name("CTFFlag::setCarrier");
	*this->_carrierId = carrierId;
	this->_carrierData = carrierData;
	this->skin->visible = (!(carrierData == nullptr));
	this->takeCommandSent = false;
	if (carrierData != nullptr)
		this->skin->alpha = ((carrierData == TankData::localTankData) ? 0.5 : 1);
	this->_state = CTFFlagState::CARRIED;
}

void CTFFlag::returnToBase()
{
	_function_name("CTFFlag::returnToBase");
	this->reset();
	this->setPosition(&this->_basePosition);
	this->_state = CTFFlagState::AT_BASE;
}

void CTFFlag::dropAt(Vector3 *dropPos, TanksCollisionDetector *collisionDetector)
{
	_function_name("CTFFlag::dropAt");
	this->reset();
	RayIntersection intersection;
	if (collisionDetector->intersectRayWithStatic(dropPos, &rayDirection, CollisionGroup::STATIC, 1, nullptr, &intersection))
		this->setPosition(&intersection.pos);
	else
		this->setPosition(dropPos);
	this->_state = CTFFlagState::DROPPED;
}

void CTFFlag::returnToNone()
{
	_function_name("CTFFlag::returnToNone");
	this->reset();
	this->_state = CTFFlagState::NONE;
}

void CTFFlag::dispose()
{
	_function_name("CTFFlag::dispose");
	//var _local_1:* = this->skin;
	//_local_1.alternativa3d::removeFromParent();
}

void CTFFlag::update(int time)
{
	_function_name("CTFFlag::update");
#ifdef GRAPHICS
	this->skin->update(time * 0.001);
#else
	(void)time;
#endif

	if (this->_carrierData != nullptr)
	{
		Tank *carrierTank = this->_carrierData->tank;
		if (carrierTank == nullptr || carrierTank->skin == nullptr)
			return;

		Mesh *turretMesh = carrierTank->skin->turretMesh;
		Matrix3 matrix;
		matrix.setRotationMatrix(turretMesh->rotation.x, turretMesh->rotation.y, turretMesh->rotation.z);
		Vector3 flagPosition;
		matrix.transformVector(&carrierTank->skin->turretDescriptor->flagMountPoint, &flagPosition);
		flagPosition.x += turretMesh->position.x;
		flagPosition.y += turretMesh->position.y;
		flagPosition.z += turretMesh->position.z - 40;
		this->setPosition(&flagPosition);
	}
}

void CTFFlag::createSkin(int frameWidth, int frameHeight, void *materials)
{
	_function_name("CTFFlag::createSkin");
	(void)frameWidth;
	(void)frameHeight;
	(void)materials;

	double w = double(frameWidth * SKIN_HEIGHT) / frameHeight;
	std::shared_ptr<AnimatedSprite3D> animSprite = new_AnimatedSprite3D(w, SKIN_HEIGHT);
#ifdef GRAPHICS
	TextureAnimation animation;
	GraphicsUtils::getTextureAnimation(&animation, this->originalTexture, frameWidth, frameHeight);
	animation.fps = 1;
	animSprite->setAnimationData(&animation);
	animSprite->setFrameIndex(1);
#endif

	animSprite->originY = 1;
	animSprite->looped = true;
	this->skin = animSprite;
	Vector3 hs(0.5 * SKIN_BASE_SIZE, 0.5 * SKIN_BASE_SIZE, 0.5 * SKIN_HEIGHT);
	this->triggerCollisionPrimitive = new CollisionBox(&hs, CollisionGroup::TANK);
#ifdef GRAPHICS
	this->hidabbleWrapper = new HidableObject3DWrapper(this->skin.get());
#endif
}

void CTFFlag::reset()
{
	_function_name("CTFFlag::reset");
	this->_carrierId->clear();
	this->_carrierData = nullptr;
	this->skin->alpha = 1;
	this->skin->visible = true;
	this->takeCommandSent = false;
}

bool CTFFlag::isAtBase()
{
	_function_name("CTFFlag::isAtBase");
	return this->skin->position.x == this->_basePosition.x && this->skin->position.y == this->_basePosition.y && this->skin->position.z == this->_basePosition.z;
}
