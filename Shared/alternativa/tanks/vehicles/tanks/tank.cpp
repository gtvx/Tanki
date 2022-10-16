#include "tank.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/math/vector3.h"
#include "alternativa/physics/bodymaterial.h"
#include "valuesmoother.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/vehicles/tanks/tankskinhull.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "alternativa/physics/collisionprimitivelistitem.h"
#include "alternativa/physics/collisionprimitivelist.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/tanks/models/battlefield/logic/updaters/HullTransformUpdater.h"
#include "alternativa/tanks/models/battlefield/logic/updaters/localhulltransformupdater.h"
#include "tankskin.h"
#include "track.h"
#include "tankskin.h"
#include "_global.h"
#include "mymath.h"
#include "tank.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "tank_bot/bot.h"
#include "service.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/object/clientobjecthull.h"
#include "alternativa/tanks/models/tank/turret/TurretController.h"
#include "gametime.h"
#include "calculatetrackspeed.h"
#include "flash/geom/matrix3d.h"
#include "alternativa/tanks/display/usertitle/usertitle.h"
#include "__tank_bot.h"


const Body::T *Tank::TYPE = new Body::T("Tank");

#define RAY_OFFSET 5


/*
static inline void vector3To3d(Vector3 *v, Vector3 *result)
{
	result->x = v->x;
	result->y = v->y;
	result->z = v->z;
}


static inline void vector3dTo3(Vector3 *v, Vector3 *result)
{
	result->x = v->x;
	result->y = v->y;
	result->z = v->z;
}
*/



void Tank::setMassAndDimensions(double mass, Vector3 *dimensions)
{
	_function_name("Tank::setMassAndDimensions");

	if (((MyMath::isNaN(mass)) || (mass <= 0)))
	{
		throw 4254;
	}
	if (((((dimensions == nullptr) || (dimensions->x <= 0)) || (dimensions->y <= 0)) || (dimensions->z <= 0)))
	{
		throw 134324;
	}
	this->mass = mass;
	this->dimensions.copy(dimensions);
	if (MyMath::isInf(mass))
	{
		invMass = 0;
		invInertia.copy(&Matrix3::ZERO);
	}
	else
	{
		//invInertia.copy(&Matrix3::ZERO);
		invMass = 1. / mass;
		double xx = (dimensions->x * dimensions->x);
		double yy = (dimensions->y * dimensions->y);
		double zz = (dimensions->z * dimensions->z);
		invInertia.a = ((12. * invMass) / (yy + zz));
		invInertia.f = ((12. * invMass) / (zz + xx));
		invInertia.k = ((12. * invMass) / (xx + yy));
	}
	this->setupCollisionPrimitives();
}

void Tank::setupCollisionPrimitives()
{
	_function_name("Tank::setupCollisionPrimitives");

	Vector3 hs;
	hs.copy(&this->dimensions);
	hs.scale(0.5);


	double sizeX = hs.x;
	double sizeY = hs.y;
	double sizeZ = hs.z;
	Matrix4 m;
	m.toIdentity();
	double sizeFactor1 = 0.8;
	if (collisionPrimitives == nullptr)
	{
		hs.y = sizeFactor1 * sizeY;
		this->mainCollisionBox = new CollisionBox(&hs, 0);
		addCollisionPrimitive(this->mainCollisionBox);
		hs.y = sizeY;
		hs.z = sizeZ / 3.;
		m.l = (2. * sizeZ) / 3.;
		this->collisionBox_2 = new CollisionBox(&hs, 0);
		addCollisionPrimitive(collisionBox_2, &m);
	}
	else
	{
		CollisionPrimitiveListItem *item = collisionPrimitives->head;
		hs.y = sizeFactor1 * sizeY;
		CollisionBox *prim = (CollisionBox*)item->primitive;
		prim->hs.copy(&hs);
		item = item->next;
		hs.y = sizeY;
		hs.z = sizeZ / 3.;
		m.l = (2. * sizeZ) / 3.;
		prim = (CollisionBox*)item->primitive;
		prim->hs.copy(&hs);
		prim->localTransform->copy(&m);
	}

	this->visibilityPoints[0].reset(-sizeX, sizeY, 0);
	this->visibilityPoints[1].reset(sizeX, sizeY, 0);
	this->visibilityPoints[2].reset(-sizeX, 0, 0);
	this->visibilityPoints[3].reset(sizeX, 0, 0);
	this->visibilityPoints[4].reset(-sizeX, -sizeY, 0);
	this->visibilityPoints[5].reset(sizeX, -sizeY, 0);
	//this.removeDebugBoxesFromContainer();
	//for (key in this.bbs)
		//delete this.bbs[key];
}


Tank::Tank(const T *type,
		   TankSkin *skin,
		   double mass,
		   double maxSpeed,
		   double maxTurnSpeed,
		   double turretMaxTurnSpeed,
		   double turretTurnAcceleration,
		   bool isLocal,
		   Scene3DContainer *titleContainer,
		   double rayLength,
		   double rayOptimalLength,
		   double dampingCoeff,
		   double friction,
		   double rays) :
	Body3D(type, 0, &Matrix3::ZERO, isLocal)
{
	_function_name("Tank::Tank");

	this->deleted = false;
	this->animatedTracks = false;
	this->turretController = nullptr;
	this->leftBrake = false;
	this->rightBrake = false;
	this->mainCollisionBox = nullptr;
	this->collisionBox_2 = nullptr;
	this->maxSpeed = 0;
	this->maxTurnSpeed = 0;
	this->maxTurretTurnSpeed = turretMaxTurnSpeed;
	this->turretTurnAcceleration = 0;
	this->turretTurnSpeed = 0;
	//this->mass = 0;
	this->leftThrottle = 0;
	this->rightThrottle = 0;
	this->movedir = 0;
	this->turndir = 0;
	this->skinZCorrection = 0;
	this->bot = nullptr;
	this->maxTurnSpeedSmoother = new ValueSmoother(0.3, 10, 0, 0);
	this->maxTurretTurnSpeedSmoother = new ValueSmoother(0.3, 10, 0, 0);
	this->skin = skin;
	this->maxSpeed = maxSpeed;
	this->maxTurnSpeed = maxTurnSpeed;
	this->turretTurnAcceleration = turretTurnAcceleration;
	this->container = nullptr;
	this->titleContainer = titleContainer;
	this->local = isLocal;

#ifdef GRAPHICS
	this->_title = new UserTitle(this->local);
	this->trackSpeed = new CalculateTrackSpeed();
#else
	this->trackSpeed = nullptr;
#endif

	this->lockedHullTransformUpdater = new LocalHullTransformUpdater(this);
	this->hullTransformUpdater = nullptr;
	this->notLockedHullUpdater = nullptr;
	this->maxSpeedSmoother = new ValueSmoother(100, 1000, 0, 0);
	Mesh *mesh = skin->hullDescriptor->mesh.get();
	mesh->calculateBounds();
	Vector3 dimensions((2 * mesh->bound.MaxX), (2 * mesh->bound.MaxY), mesh->bound.MaxZ);
	this->setMassAndDimensions(mass, &dimensions);
	this->createTracks(rays, dimensions.y * 0.8, dimensions.x - 40);
	this->suspensionData.rayLength = rayLength;
	this->suspensionData.rayOptimalLength = rayOptimalLength;
	this->suspensionData.dampingCoeff = dampingCoeff;
	this->material.friction = friction;
	this->skinZCorrection = ((-0.5 * this->dimensions.z) - this->suspensionData.rayOptimalLength) + RAY_OFFSET;
	this->botSettings = nullptr;
	this->new_contact_normal = false;
	this->normal_contact_new = false;
}

Tank::~Tank()
{
	_function_name("Tank::~Tank");

	if (mainCollisionBox != nullptr)
		delete mainCollisionBox;
	if (collisionBox_2 != nullptr)
		delete collisionBox_2;

	if (this->turretController != nullptr)
		delete this->turretController;
#ifdef TANK_BOT
	if (bot != nullptr)
		delete bot;
#endif
	delete tankData;
	delete skin;
	delete maxTurnSpeedSmoother;
	delete maxTurretTurnSpeedSmoother;
	delete maxSpeedSmoother;
	if (notLockedHullUpdater != nullptr)
		delete notLockedHullUpdater;
	delete lockedHullTransformUpdater;
#ifdef GRAPHICS
	delete this->_title;
#endif
	if (leftTrack  != nullptr)
		delete leftTrack;
	if (rightTrack  != nullptr)
		delete rightTrack;

	if (trackSpeed != nullptr)
		delete trackSpeed;
}

void Tank::setTurretController(TurretController *controller)
{
	_function_name("Tank::setTurretController");

	this->turretController = controller;
}

void Tank::setHullTransformUpdater(HullTransformUpdater *hull_)
{
	_function_name("Tank::setHullTransformUpdater");

	this->hullTransformUpdater = hull_;
	this->notLockedHullUpdater = hull_;
	this->hullTransformUpdater->reset();
}

TurretController* Tank::getTurretController()
{
	_function_name("Tank::getTurretController");
	return this->turretController;
}

void Tank::createTracks(int raysNum, double trackLength, double widthBetween)
{
	_function_name("Tank::createTracks");

	Vector3 relPos_left((-0.5 * widthBetween), 0, ((-0.5 * this->dimensions.z) + RAY_OFFSET));
	Vector3 relPos_right((0.5 * widthBetween), 0, ((-0.5 * this->dimensions.z) + RAY_OFFSET));
	this->leftTrack = new Track(this, raysNum, &relPos_left, trackLength);
	this->rightTrack = new Track(this, raysNum, &relPos_right, trackLength);
}


void Tank::setMaxSpeed(double value, bool immediate)
{
	_function_name("Tank::setMaxSpeed");

	if (immediate)
	{
		this->maxSpeed = value;
		this->maxSpeedSmoother->reset(value);
	}
	else
	{
		this->maxSpeedSmoother->setTargetValue(value);
	}
}

void Tank::setMaxTurnSpeed(double value, bool immediate)
{
	_function_name("Tank::setMaxTurnSpeed");

	if (immediate)
	{
		this->maxTurnSpeed = value;
		this->maxTurnSpeedSmoother->reset(value);
	}
	else
	{
		this->maxTurnSpeedSmoother->setTargetValue(value);
	}
}

void Tank::setMaxTurretTurnSpeed(double value, bool immediate)
{
	_function_name("Tank::setMaxTurretTurnSpeed");

	if (immediate)
	{
		this->maxTurretTurnSpeed = value;
		this->maxTurretTurnSpeedSmoother->reset(value);
	}
	else
	{
		this->maxTurretTurnSpeedSmoother->setTargetValue(value);
	}
}

void Tank::getPhysicsState(Vector3 *pos, Vector3 *orient, Vector3 *linVel, Vector3 *angVel)
{
	_function_name("Tank::getPhysicsState");
	pos->copy(&state.pos);
	state.orientation.getEulerAngles(orient);
	linVel->copy(&state.velocity);
	angVel->copy(&state.rotation);
}

int Tank::getCollisionGroup()
{
	_function_name("Tank::getCollisionGroup");

	return collisionPrimitives->head->primitive->collisionGroup;
}

void Tank::setCollisionGroup(int value)
{
	_function_name("Tank::setCollisionGroup");

	CollisionPrimitiveListItem *item = collisionPrimitives->head;
	while (item != nullptr)
	{
		item->primitive->collisionGroup = value;
		item = item->next;
	}
}

void Tank::setTracksCollisionGroup(int value)
{
	_function_name("Tank::setTracksCollisionGroup");

	this->leftTrack->setCollisionGroup(value);
	this->rightTrack->setCollisionGroup(value);
}


void Tank::setBrakes(bool lb, bool rb)
{
	_function_name("Tank::setBrakes");
	this->leftBrake = lb;
	this->rightBrake = rb;
}

void Tank::setThrottle(double throttleLeft, double throttleRight)
{
	_function_name("Tank::setThrottle");

	this->leftThrottle = throttleLeft;
	this->rightThrottle = throttleRight;
}

void Tank::setPhysicsState(const Vector3 *pos, const Vector3 *orient, const Vector3 *linVel, const Vector3 *angVel)
{
	_function_name("Tank::setPhysicsState");
	state.pos.copy(pos);
	Quaternion _orient;
	_orient.setFromAxisAngleComponents(1, 0, 0, orient->x);
	state.orientation.copy(&_orient);
	_orient.setFromAxisAngleComponents(0, 1, 0, orient->y);
	state.orientation.append(&_orient);
	state.orientation.normalize();
	_orient.setFromAxisAngleComponents(0, 0, 1, orient->z);
	state.orientation.append(&_orient);
	state.orientation.normalize();
	state.velocity.copy(linVel);
	state.rotation.copy(angVel);
	prevState.copy(&state);
}

int Tank::getTurretDirSign()
{
	_function_name("Tank::getTurretDirSign");

	return (this->skin->turretDirection < 0.) ? -1 : ((this->skin->turretDirection > 0.) ? 1 : 0);
}

void Tank::stopTurret()
{
	_function_name("Tank::stopTurret");

	this->turretTurnSpeed = 0;
}

bool Tank::rotateTurret(double timeFactor, bool stopAtZero)
{
	_function_name("Tank::rotateTurret");

	bool sign = this->skin->turretDirection < 0;
	this->turretTurnSpeed = this->turretTurnSpeed + (timeFactor * this->turretTurnAcceleration);
	if (this->turretTurnSpeed < -(this->maxTurretTurnSpeed))
	{
		this->turretTurnSpeed = -(this->maxTurretTurnSpeed);
	}
	else
	{
		if (this->turretTurnSpeed > this->maxTurretTurnSpeed)
		{
			this->turretTurnSpeed = this->maxTurretTurnSpeed;
		}
	}

	this->setTurretDir(this->skin->turretDirection + (this->turretTurnSpeed * ((timeFactor < 0) ? -(timeFactor) : timeFactor)));

	if (((stopAtZero) && (!(sign == (this->skin->turretDirection < 0)))))
	{
		this->skin->turretDirection = this->turretTurnSpeed = 0;
		return true;
	}
	return false;
}


void Tank::updateSkin(double t)
{
	_function_name("Tank::updateSkin");

	//this->leftThrottle = 100000;
	//this->rightThrottle = 100000;

	//static Vector3 _pos(0, 0, 0);

	Vector3 _pos(0, 0, 0);
	this->interpolatePhysicsState(t);
	_pos.x = _pos.x + (this->skinZCorrection * 0.); //_m.c
	_pos.y = _pos.y + (this->skinZCorrection * 0.); //_m.g
	_pos.z = _pos.z + (this->skinZCorrection * 1.); //_m.k
	Quaternion _orient;
	this->skin->updateTransform(&_pos, &_orient);


	Mesh *turretMesh = this->skin->turretMesh;

	Matrix4 m;
	m.setMatrix(turretMesh->position.x,
				turretMesh->position.y,
				turretMesh->position.z,
				turretMesh->rotation.x,
				turretMesh->rotation.y,
				turretMesh->rotation.z);
	Matrix3D m3;
	m3.set(m.ma, m.me, m.mi, 0, m.mb, m.mf, m.mj, 0, m.mc, m.mg, m.mk, 0, m.md, m.mh, m.ml, 1);

	const double vin[3] = {0, 0, 0};
	double vout[3];

	m3.transformVectors_1(vin, vout);
	_pos.x = vout[0];
	_pos.y = vout[1];
	_pos.z = vout[2];
	_pos.z += this->local ? 0 : 200;

#ifdef GRAPHICS
	this->_title->update(&_pos);
	if (this->animatedTracks)
	{
		double avel = MyMath::abs(this->state.rotation.z);
		double Speed = this->getVelocity();
		double left = this->trackSpeed->calcTrackSpeed(Speed,
											 avel,
											 this->tankData->ctrlBits,
											 this->leftTrack->lastContactsNum,
											 this->contactsNum, this->maxSpeed, -1);

		double right = this->trackSpeed->calcTrackSpeed(Speed, avel,
													   this->tankData->ctrlBits,
													   this->rightTrack->lastContactsNum,
													   this->contactsNum, this->maxSpeed, 1);

		this->skin->updateTracks(left, right);
	}
#endif
}

double Tank::getVelocity()
{
	return state.velocity.length();
}

void Tank::interpolatePhysicsState(double t)
{
	_function_name("Tank::interpolatePhysicsState");

	interpolate(t, &this->interpolatedPosition, &this->interpolatedOrientation);
	this->interpolatedOrientation.normalize();
	if (this->hullTransformUpdater != nullptr)
		this->hullTransformUpdater->update(t);
}

void Tank::calculateSkinCenterOffset(Vector3 *param1)
{
	_function_name("Tank::calculateSkinCenterOffset");

	Mesh *mesh = this->skin->hullMesh;
	mesh->calculateBounds();
	this->skinCenterOffset.x = -0.5 * (mesh->bound.MinX + mesh->bound.MaxX);
	this->skinCenterOffset.y = -0.5 * (mesh->bound.MinY + mesh->bound.MaxY);
	this->skinCenterOffset.z = ((-0.5 * param1->z) - this->suspensionData.rayOptimalLength) + 10;
}


void Tank::addToContainer(Scene3DContainer *container)
{
	_function_name("Tank::addToContainer");
	(void)container;

	if (this->container != nullptr)
		this->removeFromContainer();

	this->container = container;
	this->skin->addToContainer(container);
#ifdef GRAPHICS
	if (this->_title != nullptr)
		this->_title->addToContainer(this->titleContainer);
#endif
	//if (this->_showCollisionGeometry)
	//this->addDebugBoxesToContainer(container);

	Vector3 raduis;
	this->calculateSizeFromMesh(this->skin->hullMesh, &raduis);
	//Vector3 *hall = new Vector3((raduis->x / 2), (raduis->y / 2), (raduis->z / 2));
	double p1 = ((2. * raduis.z) - (this->suspensionData.rayOptimalLength - 10.));
	this->setBoundSphereRadius(&raduis, p1);
	this->calculateSkinCenterOffset(&raduis);
}



void Tank::removeFromContainer()
{
	this->skin->removeFromContainer();
#ifdef GRAPHICS
	if (this->_title != nullptr)
		this->_title->removeFromContainer();
#endif
	//if (this->_showCollisionGeometry)
		//this->removeDebugBoxesFromContainer();

	this->container = nullptr;
}


void Tank::calculateSizeFromMesh(Mesh *mesh, Vector3 *out)
{
	_function_name("Tank::calculateSizeFromMesh");

	out->x = mesh->bound.MaxX - mesh->bound.MinX;
	out->y = mesh->bound.MaxY - mesh->bound.MinY;
	out->z = mesh->bound.MaxZ - mesh->bound.MinZ;
}

void Tank::setBoundSphereRadius(Vector3 *radius, double p1)
{
	_function_name("Tank::setBoundSphereRadius");

	Vector3 v(radius->x, radius->y, p1 / 2.);
	const Matrix4 &transform = this->mainCollisionBox->transform;
	this->shpereRadius = v.length() + MyMath::abs(transform.k);
}

void Tank::beforePhysicsStep(double dt)
{
	_function_name("Tank::beforePhysicsStep");

	if (this->maxSpeed != this->maxSpeedSmoother->getTargetValue())
	{
		this->maxSpeed = this->maxSpeedSmoother->update(dt);
	}
	if (this->maxTurnSpeed != this->maxTurnSpeedSmoother->getTargetValue())
	{
		this->maxTurnSpeed = this->maxTurnSpeedSmoother->update(dt);
	}
	if (this->maxTurretTurnSpeed != this->maxTurretTurnSpeedSmoother->getTargetValue())
	{
		this->maxTurretTurnSpeed = this->maxTurretTurnSpeedSmoother->update(dt);
	}


	int slipTerm = (this->leftThrottle > this->rightThrottle) ? -1 : ((this->leftThrottle < this->rightThrottle) ? 1 : 0);
	double weight = this->mass * world->_gravity.length();
	double k = (((!(this->leftThrottle == this->rightThrottle)) && (!((this->leftBrake) || (this->rightBrake)))) && (state.rotation.length() > this->maxTurnSpeed)) ? 0.1 : 1;

	this->leftTrack->addForces(dt, (k * this->leftThrottle), this->maxSpeed, slipTerm, weight, &this->suspensionData, this->leftBrake);
	this->rightTrack->addForces(dt, (k * this->rightThrottle), this->maxSpeed, slipTerm, weight, &this->suspensionData, this->rightBrake);


	if (((this->rightTrack->lastContactsNum >= (this->rightTrack->raysNum >> 1)) || (this->leftTrack->lastContactsNum >= (this->leftTrack->raysNum >> 1))))
	{
		double d = (world->_gravity.x * baseMatrix.c) + (world->_gravity.y * baseMatrix.g) + (world->_gravity.z * baseMatrix.k);
		double limit = MyMath::SQRT1_2 * world->_gravity.length();
		if ((d < -limit) || (d > limit))
		{
			Vector3 v;
			v.x = ((baseMatrix.c * d) - world->_gravity.x) / invMass;
			v.y = ((baseMatrix.g * d) - world->_gravity.y) / invMass;
			v.z = ((baseMatrix.k * d) - world->_gravity.z) / invMass;
			addForce(&v);
		}
	}
}

void Tank::lockTransformUpdate()
{
	_function_name("Tank::lockTransformUpdate");
	this->hullTransformUpdater = this->lockedHullTransformUpdater;
	this->hullTransformUpdater->reset();
}

void Tank::unlockTransformUpdate()
{
	_function_name("Tank::unlockTransformUpdate");
	this->hullTransformUpdater = this->notLockedHullUpdater;
	this->hullTransformUpdater->reset();
}


double Tank::getTurretDir()
{
	_function_name("Tank::getTurretDir");

	return this->skin->turretDirection;
}

void Tank::setTurretDir(double value)
{
	_function_name("Tank::setTurretDir");

	if (this->skin == nullptr)
		return;

	if (value > MyMath::PI)
		this->skin->turretDirection = value - MyMath::PI2;
	else
		if (value < -MyMath::PI)
			this->skin->turretDirection = value + MyMath::PI2;
		else
			this->skin->turretDirection = value;
}

void Tank::destroy()
{
	_function_name("Tank::destroy");
	delete this;
}

void Tank::spawn()
{
#ifdef TANK_BOT
	if (bot != nullptr)
		bot->spawnTank(GetGameTime());
#endif
}

Bot* Tank::initBot()
{
#ifdef TANK_BOT
	TanksCollisionDetector *collisionDetector = Service::getBattlefieldModel()->getBattlefieldData()->collisionDetector;
	return bot = new Bot(Service::getTankModel(), structTank, botSettings, collisionDetector);
#else
	return nullptr;
#endif
}

void Tank::updateBot(uint32_t time)
{
#ifdef TANK_BOT
	bot->run(time);
#else
	(void)time;
#endif
}

void Tank::setAnimationTracks(bool value)
{
	this->animatedTracks = value;
}

