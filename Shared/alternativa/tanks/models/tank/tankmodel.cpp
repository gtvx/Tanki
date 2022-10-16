#include "tankmodel.h"
#include "tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/tankparts.h"
#include "alternativa/tanks/vehicles/tanks/tankskinhull.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/ClientTank.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/models/weapon/IWeapon.h"
#include "alternativa/tanks/models/battlefield/logic/updaters/remotehulltransformupdater.h"
#include "resource/tanks/tankresource.h"
#include "alternativa/tanks/vehicles/tanks/tankskinturret.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/tanks/models/tank/turret/TurretController.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/object/clientobjectuser.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/object/clientobjecthull.h"
#include "alternativa/tanks/models/tank/turret/default/defaultturretcontroller.h"
#include "alternativa/tanks/models/battlefield/logic/updaters/localhulltransformupdater.h"
#include "alternativa/tanks/models/weapon/common/weaponcommonmodel.h"
#include "alternativa/tanks/models/tank/explosion/tankdeathgraphiceffect.h"
#include "gametime.h"
#include "service.h"
#include "mymath.h"
#include "networking/network.h"
#include "keys.h"
#include "IMain.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/deathreason.h"
#include "alternativa/tanks/camera/IFollowCameraController.h"
#include "double_to_string.h"
#include "alternativa/tanks/models/ctf/ctfmodel.h"
#include "alternativa/object/clientobjectweapon.h"
#include "alternativa/object/clientobjecthull.h"
#include "alternativa/tanks/display/usertitle/defaultusertitlesrender.h"
#include "alternativa/tanks/display/usertitle/usertitle.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/models/tank/criticalhit/tankcriticalhitmodel.h"
#include "alternativa/tanks/models/tank/explosion/tankexplosionmodel.h"
#include "alternativa/tanks/models/effectsvisualization/clientbattleeffect.h"
#include "alternativa/tanks/models/effectsvisualization/IEffectsVisualizationModel.h"
#include "alternativa/tanks/models/effectsvisualization/effectsvisualizationmodel.h"
#include "_global.h"
#include "__global.h"
#include "battlecontroller.h"
#include "tanks.h"
#include "flash/utils/gettimer.h"
#include "alternativa/tanks/models/battlefield/effects/damageeffect.h"
#include "alternativa/tanks/models/tank/TankEvent.h"
#include "alternativa/tanks/models/tank/tankeventdispatcher.h"
#include "alternativa/tanks/models/tank/criticalhit/tankcriticalhitgraphiceffect.h"
#include "alternativa/tanks/models/battlefield/effects/levelup/levelupeffect.h"
#include "pro/RotateTurretCommand.h"
#include <QString>
#include <QVector>
#include <QHash>
#include <QDebug>


#define MIN_ALLOWED_Z -10000
#define DEFAULT_TANK_MASS 1250
#define DEFAULT_TANK_POWER 80000
#define RESPAWN_DELAY 3000
#define SUICIDE_DELAY 10000


#ifdef L_OT
#define CORRECTION_INTERVAL 750
#else
#define CORRECTION_INTERVAL 4000
#endif


const int TankModel::FORWARD = 1;
const int TankModel::BACK = 2;
const int TankModel::LEFT = 4;
const int TankModel::RIGHT = 8;

const int TankModel::TURRET_LEFT = 16;
const int TankModel::TURRET_RIGHT = 32;
const int TankModel::CENTER_TURRET = 64;
const int TankModel::REVERSE_TURN_BIT = 128;


TankModel::TankModel()
{
	_function_name("TankModel::TankModel");

	controlBits = 0;
	firstSpawn = false;
	idleTime = 0;
	correctionEnabled = false;
	readyToSpawnCommandSent = false;
	idleTimerEnabled = false;
	idleKickPeriod = 0;
	reverseBackTurn = false;
	awaitingKillCommand = false;
	activationTime = 0;
	nextCorrectionTime = 0;
	paused = false;
	controlsLocked = false;
	tankDataById = new QHash<QString, TankData*>;
#ifdef GRAPHICS
	userTitlesRender = new DefaultUserTitlesRender();
	damageEffect = new DamageEffect;
#endif
	numTanks = 0;
	battlefield = nullptr;
	uiControlLockCount = 0;
	localUserData = nullptr;
	suicideTime = 0;
	possz = 0;
	orientations = 0;
	linVels = 0;
	angVels = 0;
	effectsModel = nullptr;
}

TankModel::~TankModel()
{
	delete tankDataById;
#ifdef GRAPHICS
	delete damageEffect;
	delete userTitlesRender;
#endif
}

void TankModel::setBattlefield(BattlefieldModel *battlefield)
{
	_function_name("TankModel::setBattlefield");
	this->battlefield = battlefield;
}

TankData *TankModel::getTankData(ClientObjectUser *clientObject)
{
	return clientObject->tankData;
}

void TankModel::update(TankData *tankData, int time, int deltaMillis, double deltaSec, double interpolationValue, const Vector3 *cameraPos)
{
	_function_name("TankModel::update");

	(void)cameraPos;
	Tank *tank = tankData->tank;
	this->rotateTurret(tankData, deltaSec);
#ifdef GRAPHICS
	this->userTitlesRender->updateTitle(tankData, cameraPos);
#endif
	tank->skin->updateColorTransform(deltaSec);
	tank->updateSkin(interpolationValue);
	if (tankData->deadTime > 0)
	{
		tankData->deadTime = tankData->deadTime - deltaMillis;
		if (tankData->deadTime <= 0)
		{
			tankData->deadTime = 0;
			this->battlefield->removeTankFromField(tankData);
		}
	}
	if (tankData->local)
	{
		this->updateLocalTank(time, deltaMillis);
	}
}

void TankModel::disableUserControls(bool stopNotification)
{
	_function_name("TankModel::disableUserControls");
	if (this->localUserData == nullptr)
		return;
	this->_userControlsEnabled = false;
	this->controlBits = 0;
	this->applyControlState(this->localUserData, 0);
	this->localUserData->weapon->disable();
	if (stopNotification == true)
		this->correctionEnabled = false;
}

void TankModel::enableUserControls()
{
	_function_name("TankModel::enableUserControls");

	if (this->localUserData == nullptr || this->localUserData->enabled == false)
		return;

	this->_userControlsEnabled = true;
	this->correctionEnabled = true;
	this->applyControlState(this->localUserData, this->controlBits);
	if (this->localUserData->spawnState == TankSpawnState::ACTIVE) {
		this->localUserData->weapon->enable();
	}
}

void TankModel::stop(TankData *tankData)
{
	_function_name("TankModel::stop");
	this->setControlState(tankData, 0);
	tankData->weapon->ownerDisabled(tankData->user);
}

void TankModel::resetIdleTimer(bool disable)
{
	if (disable == true)
		this->idleTimerEnabled = false;
	this->idleTime = 0;
}

bool TankModel::getUserControlsEnabled()
{
	return _userControlsEnabled;
}

void TankModel::readLocalTankPosition(Vector3 *pos)
{
	_function_name("TankModel::readLocalTankPosition");
	if (this->localUserData == nullptr)
		return;
	pos->copy(&this->localUserData->tank->state.pos);
}

void TankModel::effectStarted(ClientObjectUser *clientObject, int effectId, int duration)
{
#ifdef GRAPHICS
	TankData *tankData = this->getTankData(clientObject);
	if (tankData == nullptr || tankData->tank == nullptr)
	{
		//Logger.log(LogLevel.LOG_ERROR, ((((("TankModel::effectStarted(): Incorrect call clientObject=" + clientObject) + ", tankData=") + tankData) + ", effectId=") + effectId));
		return;
	}

	tankData->tank->title()->showIndicator(effectId, duration);
#else
	(void)clientObject;
	(void)effectId;
	(void)duration;
#endif
}

void TankModel::effectStopped(ClientObjectUser *clientObject, int effectId)
{
#ifdef GRAPHICS
	TankData *tankData = this->getTankData(clientObject);
	if (tankData == nullptr || tankData->tank == nullptr)
	{
		//Logger.log(LogLevel.LOG_ERROR, ((((("TankModel::effectStopped(): Incorrect call clientObject=" + clientObject) + ", tankData=") + tankData) + ", effectId=") + effectId));
		return;
	}
	tankData->tank->title()->hideIndicator(effectId);
#else
	(void)clientObject;
	(void)effectId;
#endif
}



void TankModel::updateLocalTank(int time, int timeDelta)
{
	_function_name("TankModel::updateLocalTank");

	if (this->localUserData->enabled)
	{
		Vector3 &pos = this->localUserData->tank->state.pos;
		if (((time >= this->suicideTime) || (pos.z < MIN_ALLOWED_Z)))
		{
			this->sendSuicideCommand(&pos);
		}

		double weaponStatus = this->localUserData->weapon->update(time, timeDelta);

#ifdef GRAPHICS
		this->localUserData->tank->title()->setWeaponStatus(100. * weaponStatus);
#else
		(void)weaponStatus;
#endif
		if ((((this->localUserData->spawnState == TankSpawnState::NEWCOME) && (time >= this->activationTime)) && (this->localUserData->tankCollisionCount == 0)))
		{
			this->activationTime = INT_MAX;
			this->activateTankCommand(this->localUserData->user);
		}

		this->localUserData->tank->updateBot(time);

	}
	if ((((this->localUserData->deadTime == 0) && (!(this->paused))) && (!(this->readyToSpawnCommandSent))))
	{
		this->readyToSpawnCommandSent = true;
	}

	//pos = this.localUserData.tank.state.pos;
	if (this->correctionEnabled && (time >= this->nextCorrectionTime))
	{
		Vector3 pos;
		Vector3 orient;
		Vector3 linVel;
		Vector3 angVel;
		this->localUserData->tank->getPhysicsState(&pos, &orient, &linVel, &angVel);

#ifndef L_OT
		this->moveCommand(this->localUserData->user, time, &pos, &orient, &linVel, &angVel, this->localUserData->tank->getTurretDir(), this->localUserData->ctrlBits);
#else
		this->moveCorrectCommand(this->localUserData->user, time, &pos, &orient, &linVel, &angVel, this->localUserData->tank->getTurretDir(), this->localUserData->ctrlBits);
#endif

		this->nextCorrectionTime = time + CORRECTION_INTERVAL;
	}





	//if (this->paused || this->idleTimerEnabled)
	//this->checkIdleKick(timeDelta);
	//if (this->paused)
	//this->gui.setPauseTimeout(((this->idleKickPeriod - this->idleTime) / 1000));
}


void TankModel::initLocalTank(TankData *tankData)
{
	_function_name("TankModel::initLocalTank");

	TankData::localTankData = tankData;
	this->idleTime = 0;
	//this->paused = false;
	//Main.stage.focus = null;
	this->localUserData = tankData;
#ifdef GRAPHICS
	this->userTitlesRender->setLocalData(this->localUserData);
#endif
	this->battlefield->setLocalUser(tankData->user);
	this->firstSpawn = true;
	this->correctionEnabled = false;
	this->_userControlsEnabled = false;
	this->idleTimerEnabled = false;
	this->idleKickPeriod = this->battlefield->getBattlefieldData()->idleKickPeriod;
	this->reverseBackTurn = false;
	this->readyToSpawnCommandSent = true;
	this->battlefield->setCameraTarget(tankData->tank);
	//tankData->health = 10000;
#ifdef GRAPHICS
	UserTitle *title = this->localUserData->tank->title();
	title->setRank(tankData->userRank);
	title->setLabelText(*tankData->userName);
	title->setHealth(tankData->health);
	title->setConfiguration(UserTitle::BIT_HEALTH | UserTitle::BIT_WEAPON | UserTitle::BIT_EFFECTS);
	title->setWeaponStatus(100);
	title->show();
#endif
}

bool TankModel::isSameTeam(BattleTeamType teamType1, BattleTeamType teamType2)
{
	return (!(teamType1 == BattleTeamType::NONE)) && (teamType1 == teamType2);
}

void TankModel::rotateTurret(TankData *tankData, double deltaSec)
{
	_function_name("TankModel::rotateTurret");
	tankData->tank->getTurretController()->rotateTurret(deltaSec);
}



Tank* TankModel::createTank(TankData *tankData, TankParts *cl)
{
	_function_name("TankModel::createTank");

	bool HD_hull = (!(cl->hullObjectId.indexOf("HD_") == -1));
	bool HD_turret = (!(cl->turretObjectId.indexOf("HD_") == -1));
	if (HD_hull)
	{
		cl->hullObjectId = cl->hullObjectId.replace("HD_", "");
	}
	if (HD_turret)
	{
		cl->turretObjectId = cl->turretObjectId.replace("HD_", "");
	}


	//Hull корпус

	TankResource *tank_resource_hull = ResourceUtil::getResourceModel(cl->hullObjectId);
	if (tank_resource_hull == nullptr)
		throw 4352342;

	TankSkinHull *hullDescriptor = new TankSkinHull(tank_resource_hull->mesh, tank_resource_hull->turretMount.get());

	TankResource *tank_resourc_turret = ResourceUtil::getResourceModel(cl->turretObjectId);
	TankSkinTurret *turretDescriptor = new TankSkinTurret(tank_resourc_turret->mesh,
														  tank_resourc_turret->flagMount.get(),
														  tank_resourc_turret->muzzles->at(0).get());

	ImageResource* colormap = nullptr;

#ifdef GRAPHICS
	colormap = ResourceUtil::getResourceImage(cl->coloringObjectId);
	if (colormap == nullptr)
	{
		qDebug("error 245243");
		return nullptr;
	}
#endif

	TankSkin *skin = new TankSkin(hullDescriptor,
								  turretDescriptor,
								  colormap,
								  (((HD_hull) ? "HD_" : "") + cl->hullObjectId) + "_lightmap",
								  (((HD_hull) ? "HD_" : "") + cl->hullObjectId) + "_details",
								  (((HD_turret) ? "HD_" : "") + cl->turretObjectId) + "_lightmap",
								  (((HD_turret) ? "HD_" : "") + cl->turretObjectId) + "_details",
								  nullptr);


	if (tankData == nullptr)
		throw 45325;
	if (this->battlefield == nullptr)
		throw 45326;

	BattlefieldData *battlefieldData = this->battlefield->getBattlefieldData();
	if (battlefieldData == nullptr)
		throw 45327;

	if (battlefieldData->viewport == nullptr)
		throw 45328;


	Tank *tank = new Tank(Tank::TYPE,
						  skin,
						  tankData->mass,
						  1, 1, 1, 1, tankData->local,
						  tankData->local ? this->battlefield->getBattlefieldData()->viewport->getFrontContainer() : this->battlefield->getBattlefieldData()->viewport->getMapContainer());
	tank->tankData = tankData;
	tankData->tank = tank;
	tank->setTurretController(new DefaultTurretController(tankData));
	return tank;
}


void TankModel::initTank(std::shared_ptr<ClientObjectUser> &clientObject, ClientTank *clientTank, TankParts *parts, bool notificationOfEnter)
{
	_function_name("TankModel::initTank");
	if (clientObject == nullptr)
		return;

	(void)notificationOfEnter;

	TankData *tankData = this->getTankData(clientObject.get());
	/*
	if (tankData == nullptr)
	{
		ResourceUtil.addEventListener(function ():void
									  {
										  initTank(clientObject, clientTank, parts, notificationOfEnter);
									  });
		return;
	}
	*/



	tankData->deadTime = 0;
	tankData->local = clientTank->self;
	Tank *tank = this->createTank(tankData, parts);
	if (tank == nullptr || tankData == nullptr)
	{
		throw 524541;
	}


	tankData->tank = tank;
	//tankData->sounds.tank = tank;
	tankData->teamType = clientTank->teamType;
	tankData->incarnation = clientTank->incarnationId;
	tankData->_user = clientObject;
	tankData->user = clientObject.get();
	this->setNormalTextures(tankData);
	this->setTankSpec(tankData, &clientTank->tankSpecification, true);
	IWeapon *tankWeapon = tankData->weapon;
	if (tankWeapon == nullptr)
		throw 21343242;


	tankData->tank->turretTurnAcceleration = 2;
	tankData->tank->maxTurretTurnSpeed = 2;
	tankWeapon->ownerLoaded(clientObject.get());
	if (clientTank->self == true)
	{
		this->localUserData = tankData;
#ifdef GRAPHICS
		this->userTitlesRender->setLocalData(this->localUserData);
#endif
		this->initLocalTank(tankData);
		this->suicideTime = INT32_MAX;
	}

	//tankData.logEvent("Initialized");
	if (clientTank->tankState.health != 0) {
		this->putTankIntoBattle(tankData, clientTank);
	} else {
		tankData->tank->state.pos.reset();
		tankData->tank->state.orientation.reset();
		tankData->tank->state.velocity.reset();
		tankData->tank->state.rotation.reset();
		tankData->tank->prevState.copy(&tankData->tank->state);
	}


	this->tankDispatcher->dispatchEvent(TankEvent::LOADED, tankData);
	this->battlefield->addTank(tankData);

	//if (notificationOfEnter)
	//StatisticsModel(Main.osgi.getService(IBattlefieldGUI)).userConnect(clientObject, tankData.userName, tankData.teamType, tankData.userName, tankData.userRank);

	if (tankData->local == false)
		this->configureRemoteTankTitle(tankData);

	if (tankData->local == true)
	{
		tankData->tank->setHullTransformUpdater(new LocalHullTransformUpdater(tankData->tank));
	}
	else
	{
		tankData->tank->setHullTransformUpdater(new RemoteHullTransformUpdater(tankData->tank));
		/*
		if (this->activateLater[clientObject.id] != nullptr)
		{
			switch (this->activateLater[clientObject.id])
			{
				case "activate":
					//this->activateTank(clientObject);
					break;
				case "spawn":
					//v = this->laterData[clientObject.id];
					//this->spawn(clientObject, TankSpecification(v[0]), BattleTeamType(v[1]), Vector3d(v[2]), Vector3d(v[3]), int(v[4]), int(v[5]));
					//this->laterData[clientObject.id] = null;
					break;
			}
			//this->activateLater[clientObject.id] = null;
		}
		*/
	}

	tank->calcDerivedData(); //fix

	TANKS::tank_add(tank);
	Service::getIMain()->initTank(tank);
}

void TankModel::activateTank(ClientObjectUser *clientObject)
{
	_function_name("TankModel::activateTank");
	if (clientObject == nullptr)
		return;


	TankData* tankData = this->getTankData(clientObject);
	if ((((((tankData == nullptr) || (tankData->tank == nullptr)) || (tankData->tank->skin == nullptr)) || (tankData->tank->skin->hullMesh == nullptr)) || (tankData->tank->skin->turretMesh == nullptr)))
	{
		//Logger.log(LogLevel.LOG_ERROR, ((("TankModel: Incorrect call of activateTank() clientObject=" + clientObject) + ", tankData=") + tankData));
		//this->activateLater[clientObject.id] = "activate";
		return;
	}

	//tankData.logEvent("Activated");
	this->setNormalState(tankData);
	if (tankData->local)
	{
		this->updateUILock(0);
	}
	this->tankDispatcher->dispatchEvent(TankEvent::ACTIVATED, tankData);
}


void TankModel::setNormalTextures(TankData *tankData)
{
	_function_name("TankModel::setNormalTextures");

	tankData->tank->skin->setNormalState();
}

void TankModel::setTankSpec(TankData *tankData, const TankSpecification *spec, bool immediate)
{
	_function_name("TankModel::setTankSpec");

	tankData->tank->setMaxSpeed((spec->speed * 100.), immediate);
	tankData->tank->setMaxTurnSpeed(spec->turnSpeed, immediate);
	tankData->tank->setMaxTurretTurnSpeed(spec->turretRotationSpeed, immediate);
}

void TankModel::putTankIntoBattle(TankData *tankData, ClientTank *clientTank)
{
	_function_name("TankModel::putTankIntoBattle");

	const TankState &tankState = clientTank->tankState;

	tankData->enabled = true;
	this->setTankHealth(tankData, clientTank->health);
	if (clientTank->health <= 0)
	{
		this->setDeadTextures(tankData);
		//tankData->tank->title->hide();
	}
	else
	{
		if (((!(TankData::localTankData == nullptr)) && (!(clientTank->self))))
		{
			this->configureRemoteTankTitle(tankData);
		}
	}

	static const Vector3 zeroVector3d(0, 0, 0);
	this->setTankState(tankData, &tankState.position, &tankState.orientation, &zeroVector3d, &zeroVector3d, tankState.turretAngle, tankState.control, true);
	switch (clientTank->spawnState)
	{
		case TankSpawnState::NEWCOME:
			this->setTransparentState(tankData);
			break;
		case TankSpawnState::ACTIVE:
			this->setNormalState(tankData);
			break;
		default:
			break;
	}
}

void TankModel::configureRemoteTankTitle(TankData *tankData)
{
#ifdef GRAPHICS
	this->userTitlesRender->configurateTitle(tankData);
#else
	(void)tankData;
#endif
}

void TankModel::sendSuicideCommand(Vector3 *pos)
{
	_function_name("TankModel::sendSuicideCommand");

	if (!this->awaitingKillCommand)
	{
		this->suicideTime = INT_MAX;
		this->awaitingKillCommand = true;
		this->suicideCommand(this->localUserData->user, DeathReason::SUICIDE, pos);
	}
}


void TankModel::activateTankCommand(ClientObjectUser *user)
{
	_function_name("TankModel::activateTankCommand");

	(void)user;
	Service::getNetworker()->send("battle;activate_tank");
}

void TankModel::setNormalState(TankData *tankData)
{
	_function_name("TankModel::setNormalState");

	tankData->spawnState = TankSpawnState::ACTIVE;
	tankData->tank->setCollisionGroup(CollisionGroup::TANK | CollisionGroup::ACTIVE_TRACK | CollisionGroup::WEAPON);
	if (tankData == TankData::localTankData)
	{
		tankData->tank->setCollisionGroup(tankData->tank->getCollisionGroup() | CollisionGroup::BONUS_WITH_TANK);
	}
	tankData->tank->setTracksCollisionGroup(CollisionGroup::ACTIVE_TRACK);
	tankData->tank->skin->hullMesh->alpha = 1;
	tankData->tank->skin->turretMesh->alpha = 1;
	tankData->tank->postCollisionPredicate = nullptr;
}


void TankModel::updateUILock(int lockCountDelta)
{
	_function_name("TankModel::updateUILock");

	(void)lockCountDelta;

	this->uiControlLockCount += lockCountDelta;
	if (this->uiControlLockCount <= 0)
	{
		this->enableUserControls();
	}
	else
	{
		this->disableUserControls(false);
	}
}

void TankModel::setControlState(TankData *tankData, int ctrlBits, bool forced)
{
	_function_name("TankModel::setControlState");

	if (((!(tankData)) || (!(tankData->enabled))))
		return;

	tankData->ctrlBits = ctrlBits;
	if (((this->controlsLocked) && (!(forced))))
		return;

	double throttle = tankData->power;
	int moveDir = (int((!((ctrlBits & FORWARD) == 0))) - int((!((ctrlBits & BACK) == 0))));
	double throttleLeft = (moveDir * throttle);
	double throttleRight = (moveDir * throttle);
	double turnDir = (int((!((ctrlBits & LEFT) == 0))) - int((!((ctrlBits & RIGHT) == 0))));
	Tank *tank = tankData->tank;
	if (moveDir == 0)
	{
		tank->setBrakes(false, false);
		double k = 0.8;
		throttleLeft = throttleLeft - ((turnDir * throttle) * k);
		throttleRight = throttleRight + ((turnDir * throttle) * k);
	}
	else
	{
		double k = 0.5;
		if (moveDir == 1)
		{
			tank->setBrakes((turnDir == 1), (turnDir == -1));
			if (turnDir == 1)
			{
				throttleLeft = (throttleLeft - (throttle * k));
			}
			if (turnDir == -1)
			{
				throttleRight = (throttleRight - (throttle * k));
			}
		}
		else
		{
			if ((ctrlBits & REVERSE_TURN_BIT) == 0)
			{
				tank->setBrakes((turnDir == -1), (turnDir == 1));
				if (turnDir == -1)
				{
					throttleLeft = (throttleLeft + (throttle * k));
				}
				if (turnDir == 1)
				{
					throttleRight = (throttleRight + (throttle * k));
				}
			}
			else
			{
				tank->setBrakes((turnDir == 1), (turnDir == -1));
				if (turnDir == 1)
				{
					throttleLeft = (throttleLeft + (throttle * k));
				}
				if (turnDir == -1)
				{
					throttleRight = (throttleRight + (throttle * k));
				}
			}
		}
	}

	tankData->tank->setThrottle(throttleLeft, throttleRight);

	/*
	if (moveDir != 0)
	{
		tankData.sounds.setAccelerationMode();
	}
	else
	{
		if (turnDir != 0)
		{
			tankData.sounds.setTurningMode();
		}
		else
		{
			tankData.sounds.setIdleMode();
		}
	}
	*/
}

void TankModel::setTankState(TankData *tankData, const Vector3 *pos, const Vector3 *orient, const Vector3 *linVel, const Vector3 *angVel, double turretDir, int control, bool isControl)
{
	_function_name("TankModel::setTankState");
	tankData->tank->setPhysicsState(pos, orient, linVel, angVel);
	this->setControlState(tankData, control, isControl);
	tankData->tank->setTurretDir(turretDir);
}


void TankModel::setTankHealth(TankData *tankData, int health)
{
	_function_name("TankModel::setTankHealth");
	tankData->health = health;
#ifdef GRAPHICS
	if (tankData->tank != nullptr)
		tankData->tank->title()->setHealth(health);
#endif
	//int maxHealth = 10000;
	//if (tankData->local)
	//this->inventoryModel->lockItem(InventoryItemType.FISRT_AID, InventoryLock.FORCED, (health >= maxHealth));
}

void TankModel::setDeadTextures(TankData *tankData)
{
	_function_name("TankModel::setDeadTextures");

	tankData->tank->skin->setDeadState();
}

void TankModel::suicideCommand(ClientObject *user, DeathReason reason, Vector3 *pos)
{
	_function_name("TankModel::suicideCommand");
	(void)user;
	(void)reason;
	(void)pos;
	Service::getNetworker()->send("battle;suicide");
}


bool TankModel::initObject(std::shared_ptr<ClientObjectUser> &clientObject,
						   const QString &battleId,
						   double mass,
						   double power,
						   TankSoundScheme *soundScheme,
						   TankParts *tankParts,
						   TankResources *tankResources,
						   double impactForce,
						   double kickback,
						   double turretRotationAcceleration,
						   double turretRotationSpeed,
						   const QString &nickname,
						   int rank,
						   const QString &turretId)
{
	_function_name("TankModel::initObject");

	//if (this->modelService == nullptr)
	{
		this->initModel();
	}
	/*
	if (Game->getInstance.classObject == null)
	{
		//errorMessage = "TankModel::initObject() battle object not found";
		//Logger.log(LogLevel.LOG_ERROR, errorMessage);
		throw 4523143;
	}
	*/

	if (ResourceUtil::isDataPresent(tankParts))
	{
		if (!this->createData(clientObject, battleId, mass, power, soundScheme, tankParts, tankResources, impactForce, kickback, turretRotationAcceleration, turretRotationSpeed, nickname, rank, turretId))
			return false;
	}

	return true;

	/*
	ResourceUtil.addEventListener(function ():void
								  {
									  createData(clientObject, battleId, mass, power, soundScheme, internal::tankParts, tankResources, impactForce, kickback, turretRotationAcceleration, turretRotationSpeed, nickname, rank, turretId);
								  });
	ResourceUtil.loadGraphicsFor(tankParts);
	*/
}


bool TankModel::createData(std::shared_ptr<ClientObjectUser> &clientObject, const QString &battleId, double mass, double power, TankSoundScheme *soundScheme, TankParts *tankParts, TankResources *tankResources, double impactForce, double kickback, double turretRotationAcceleration, double turretRotationSpeed, const QString &nickname, int rank, const QString &turretId)
{
	_function_name("TankModel::createData");

	(void)battleId;
	(void)soundScheme;
	(void)tankParts;
	(void)tankResources;

	TankData *tankData = new TankData();
	//tankData->logEvent("Loaded");
	tankData->_user = clientObject;
	tankData->user = clientObject.get();
	tankData->spawnState = TankSpawnState::NEWCOME;
	*tankData->userName = nickname;
	tankData->userRank = rank;
	tankData->mass = (mass <= 0) ? DEFAULT_TANK_MASS : mass;
	tankData->power = (power <= 0) ? DEFAULT_TANK_POWER : power;
	tankData->_hull = this->initClientObject(nickname + "_hull", nickname + "_hull");
	tankData->hull = tankData->_hull.get();
	tankData->turret = WeaponsManager::getObjectFor(turretId);
	if (tankData->turret == nullptr)
		return false;
	tankData->weapon = WeaponsManager::getWeapon(clientObject.get(), tankData->turret, impactForce, kickback, turretRotationAcceleration, turretRotationSpeed);
	if (tankData->weapon == nullptr)
		return false;


	//this->initTankColoring(objectsRegister, tankData, tankParts);
	//tankData->deadColoring = new BitmapData(100, 100);
	//tankData->sounds = this->createTankSounds(soundScheme);
	clientObject->tankData = tankData;
	this->tankDataById->insert(clientObject->getId(), tankData);
	this->numTanks++;
	/*
	if (this->numTanks == 1)
	{
		dumpService = IDumpService(Main.osgi.getService(IDumpService));
		if (dumpService != null)
		{
			dumpService.registerDumper(this);
		}
	}
	*/
	//this->battlefield = (Main.osgi.getService(IBattleField) as BattlefieldModel);
	//this->userTitlesRender = (((Main.osgi.getService(IBattleField) as BattlefieldModel).spectatorMode) ? new SpectatorUserTitlesRender() : new DefaultUserTitlesRender());


#ifdef GRAPHICS
	this->userTitlesRender->setBattlefield(this->battlefield);

	if (!TankExplosionModel::initObject(tankData->hull, "explosionTexture", "shockWaveTexture", "smokeTexture"))
		return false;

	if (!TankCriticalHitModel::initObject(tankData->hull, "explosionTexture", "shockWaveTexture", "smokeTexture"))
		return false;
#endif

	return true;
}


std::shared_ptr<ClientObjectHull> TankModel::initClientObject(const QString &id, const QString &name)
{
	_function_name("TankModel::initClientObject");
	return std::make_shared<ClientObjectHull>(id, nullptr, name, nullptr);
}

void TankModel::spawn(ClientObjectUser *clientObject, TankSpecification *spec, BattleTeamType teamType,
					  Vector3 *pos, Vector3 *orientation, int health, int incarnationId)
{
	_function_name("TankModel::spawn");

	if (clientObject == nullptr)
		return;

	TankData *tankData = this->getTankData(clientObject);

	if (tankData == nullptr || tankData->tank == nullptr) {
		qDebug() << clientObject->getId();
		qDebug("error TankModel::spawn");
		return;
	}


	/*
	if ((((((tankData == null) || (tankData.tank == null)) || (tankData.tank.skin == null)) || (tankData.tank.skin.hullMesh == null)) || (tankData.tank.skin.turretMesh == null)))
	{
		Logger.log(LogLevel.LOG_ERROR, ((("TankModel: Incorrect call of spawn() clientObject=" + clientObject) + ", tankData=") + tankData));
		if (((clientObject == null) || (clientObject.id == null)))
		{
			return;
		}
		this->activateLater[clientObject.id] = "spawn";
		this->laterData[clientObject.id] = new Vector.<Object>();
		this->laterData[clientObject.id].push(spec);
		this->laterData[clientObject.id].push(teamType);
		this->laterData[clientObject.id].push(pos);
		this->laterData[clientObject.id].push(orientation);
		this->laterData[clientObject.id].push(health);
		this->laterData[clientObject.id].push(incarnationId);
		return;
	}
	tankData.logEvent("Spawn begin");
	*/


	//Tank *tank = tankData->tank;
	/*
	if (((((tankData->local) && (!(teamType == BattleTeamType::NONE))) && (!(teamType == tankData->teamType))) || (((tankData->local) && (this->firstSpawn)) && (!(teamType == BattleTeamType::NONE)))))
	{
		ChangeTeamAlert *changeTeamAlert = new ChangeTeamAlert(CHANGE_TEAM_ALERT_DELAY, ((teamType == BattleTeamType.RED) ? ChangeTeamAlert.RED : ChangeTeamAlert.BLUE));
		changeTeamAlert.x = ((Main.stage.stageWidth - changeTeamAlert.width) >> 1);
		changeTeamAlert.y = ((Main.stage.stageHeight - changeTeamAlert.height) >> 1);
		Main.stage.addChild(changeTeamAlert);
	}
	*/

	tankData->deadTime = 0;

	tankData->enabled = true;
	tankData->incarnation = incarnationId;
	this->setTankHealth(tankData, health);
	tankData->teamType = teamType;
#ifdef GRAPHICS
	tankData->tank->title()->setTeamType(teamType);
#endif
	this->setTankSpec(tankData, spec, true);
	tankData->tank->skin->resetColorTransform();
	pos->z = pos->z + 200;
	tankData->tank->clearAccumulators();
	Vector3 zeroVector3d(0, 0, 0);
	this->setTankState(tankData, pos, orientation, &zeroVector3d, &zeroVector3d, 0, 0, true);
	this->setNormalTextures(tankData);
	//tankData.sounds.setIdleMode();
	this->setTransparentState(tankData);
	if (tankData->local == true)
	{
		tankData = this->localUserData;
		this->idleTimerEnabled = true;
		this->activationTime = GetGameTime() + this->battlefield->getRespawnInvulnerabilityPeriod();
		this->suicideTime = INT_MAX;
		this->awaitingKillCommand = false;
#ifdef GRAPHICS
		tankData->tank->title()->show();
#endif
		tankData->weapon->reset();
		this->updateUILock(0);
		this->configureOtherTankTitles();
		this->firstSpawn = false;
		this->controlsLocked = false;
		tankData->tank->spawn();
		//this->moveCorrectCommand(this->localUserData->user, 0, pos, orientation, this->_linVel3d, this._angVel3d, this->localUserData->tank->getTurretDir(), this->localUserData->ctrlBits);
	}
	else
	{
		this->configureRemoteTankTitle(tankData);
	}
	//tankData.logEvent("Spawned");
	this->tankDispatcher->dispatchEvent(TankEvent::SPAWNED, tankData);
	this->battlefield->spawnTank(tankData);
	if (!tankData->local)
		tankData->tank->unlockTransformUpdate();

	tankData->tank->updateSkin(1);
}




void TankModel::configureOtherTankTitles()
{
#ifdef GRAPHICS
	BattlefieldData *bfData = this->battlefield->getBattlefieldData();
	QHashIterator<TankData*, bool> i(*bfData->activeTanks);
	while (i.hasNext())
	{
		i.next();
		TankData *otherTankData = i.key();
		if (otherTankData != this->localUserData || !otherTankData->local)
		{
			this->configureRemoteTankTitle(otherTankData);

			QVector<ClientBattleEffect*> effects = this->effectsModel->getInitialEffects(otherTankData->user->getId());
			if (!effects.isEmpty())
			{
				int now = getTimer();
				for (ClientBattleEffect *effect : effects)
				{
					int delta = now - effect->receiveTime;
					int duration = effect->duration - delta;
					if (duration > 0)
					{
						otherTankData->tank->title()->showIndicator(effect->effectId, duration);
					}
				}
			}
		}
	}
#endif
}

void TankModel::setTransparentState(TankData *tankData)
{
	_function_name("TankModel::setTransparentState");

	tankData->spawnState = TankSpawnState::NEWCOME;
	tankData->tank->setCollisionGroup(CollisionGroup::TANK);
	tankData->tank->setTracksCollisionGroup(CollisionGroup::INACTIVE_TRACK);
	tankData->tank->skin->hullMesh->alpha = 0.5;
	tankData->tank->skin->turretMesh->alpha = 0.5;
	tankData->tank->postCollisionPredicate = (IBodyCollisionPredicate*)this->battlefield;
}



void TankModel::prepareToSpawn(ClientObjectUser *clientObject, Vector3 *pos, Vector3 *orientation)
{
	_function_name("TankModel::prepareToSpawn");

	if (clientObject == nullptr)
		throw 54342;

	TankData* tankData = this->getTankData(clientObject);
	if (tankData == nullptr)
	{
		//Logger.info(LogLevel.LOG_ERROR, ((("TankModel: Incorrect call of prepareToSpawn() clientObject=" + clientObject) + ", tankData=") + tankData));
		qDebug() << "error TankModel::prepareToSpawn" << clientObject << clientObject->getId();
		throw 5434;
		return;
	}

	if (tankData->tank == nullptr) {
		throw 5435;
	}


	//tankData->logEvent("Prepare to spawn");
	tankData->deadTime = 0;

	this->battlefield->removeTankFromField(tankData);

	if (tankData->local == false)
		tankData->tank->lockTransformUpdate();

	if (((!(this->localUserData == nullptr)) && (clientObject == this->localUserData->user)))
	{
		Vector3 pivotPosition(pos->x, pos->y, pos->z + 200);
		Vector3 targetDirection(-MyMath::sin(orientation->z), MyMath::cos(orientation->z), 0);
		if (this->firstSpawn)
		{
			this->battlefield->initFollowCamera(&pivotPosition, &targetDirection);
		}
		else
		{
			this->battlefield->initFlyCamera(&pivotPosition, &targetDirection);
		}
		this->correctionEnabled = false;
	}



}


void TankModel::applyControlState(TankData *tankData, int ctrlBits, bool sendToServer)
{
	_function_name("TankModel::applyControlState");

	if (tankData->enabled == true && tankData->ctrlBits != ctrlBits)
	{
		this->setControlState(tankData, ctrlBits);
		int time = GetGameTime();
		if (sendToServer || 0 /*(tankData.tank.getTurretController() is ShaftTurretController)) */)
		{
			Vector3 pos3d, orient3d, linVel3d, angVel3d;
			tankData->tank->getPhysicsState(&pos3d, &orient3d, &linVel3d, &angVel3d);
			this->moveCommand(this->localUserData->user, time, &pos3d, &orient3d, &linVel3d, &angVel3d, tankData->tank->getTurretDir(), ctrlBits);
			this->nextCorrectionTime = time + CORRECTION_INTERVAL;
		}
	}
}



void TankModel::moveCommand(ClientObjectUser *obj, int time, const Vector3 *pos, const Vector3 *orientation, const Vector3 *linVel, const Vector3 *angVel, double turretDir, int controlBits)
{
	_function_name("TankModel::moveCommand");

	(void)obj;
	(void)time;

	if (this->controlsLocked)
	{
		controlBits = 0;
		int turretRotationDir = (((this->localUserData->ctrlBits & TankModel::TURRET_LEFT) || (this->localUserData->ctrlBits & TankModel::LEFT)) >> 4) - (((this->localUserData->ctrlBits & TankModel::TURRET_RIGHT) || (this->localUserData->ctrlBits & TankModel::RIGHT)) >> 5);
		int turretRotationDirAddition = (((this->localUserData->ctrlBits & TankModel::LEFT) >> 2) - ((this->localUserData->ctrlBits & TankModel::RIGHT) >> 3));
		if (turretRotationDir == 0)
		{
			turretRotationDir = turretRotationDirAddition;
		}

		if (turretRotationDir == 1)
		{
			controlBits = TankModel::TURRET_LEFT;
		}
		else
		{
			if (turretRotationDir == -1)
			{
				controlBits = TankModel::TURRET_RIGHT;
			}
			else
			{
				if (((turretRotationDir == 0) && (!((this->localUserData->ctrlBits & TankModel::CENTER_TURRET) == 0))))
				{
					controlBits = TankModel::CENTER_TURRET;
				}
			}
		}
	}

	//battle;move;
	//-1824.021@-3360.458@94.24999877031566@-3.639579054219695e-17@9.82163814908179e-19@-1.5703703244421807@5.316081823431715e-15@-1.5846197714851817e-16@0.000005786643786475142@3.3876369146271307e-16@6.791729481371218e-18@-1.8606377965942528e-32;0;0;1

#ifdef L_PRO
	return;
#endif

	QString s;
	s.append("battle;move;");
	s.append(double_to_string(pos->x) + QChar('@'));
	s.append(double_to_string(pos->y) + QChar('@'));
	s.append(double_to_string(pos->z) + QChar('@'));

	s.append(double_to_string(orientation->x) + QChar('@'));
	s.append(double_to_string(orientation->y) + QChar('@'));
	s.append(double_to_string(orientation->z) + QChar('@'));

	s.append(double_to_string(linVel->x) + QChar('@'));
	s.append(double_to_string(linVel->y) + QChar('@'));
	s.append(double_to_string(linVel->z) + QChar('@'));

	s.append(double_to_string(angVel->x) + QChar('@'));
	s.append(double_to_string(angVel->y) + QChar('@'));
	s.append(double_to_string(angVel->z) + QChar('@'));

	s.append(QChar(';') + QString::number(turretDir) + QChar(';') + QString::number(controlBits) + ";1");

	Service::getNetworker()->send(s);
}



void TankModel::onKey(int key, bool down)
{
	//_function_name("TankModel::TankModel");

	bool c = false;
	switch (key)
	{
		case KEY_LEFT:
		case KEY_A:
			if (down == true)
				this->controlBits |= LEFT;
			else
				this->controlBits &= ~LEFT;
			c = true;
			break;

		case KEY_RIGHT:
		case KEY_D:
			if (down == true)
				this->controlBits |= RIGHT;
			else
				this->controlBits &= ~RIGHT;
			c = true;
			break;

		case KEY_UP:
		case KEY_W:
			if (down == true)
				this->controlBits |= FORWARD;
			else
				this->controlBits &= ~FORWARD;
			c = true;
			break;

		case KEY_DOWN:
		case KEY_S:
			if (down == true)
				this->controlBits |= BACK;
			else
				this->controlBits &= ~BACK;
			c = true;
			break;

		case KEY_Z:
		case KEY_BC:
			if (down == true)
				this->controlBits |= TURRET_LEFT;
			else
				this->controlBits &= ~TURRET_LEFT;
			c = true;
			break;

		case KEY_X:
		case KEY_BE:
			if (down == true)
				this->controlBits |= TURRET_RIGHT;
			else
				this->controlBits &= ~TURRET_RIGHT;
			c = true;
			break;

		case KEY_C:
		case KEY_BF:
			if (down == true)
				this->controlBits |= CENTER_TURRET;
			c = true;
			break;
		case KEY_DELETE:
			if (down == true)
				this->startSuicideCountdown();
			break;
		case KEY_F:
			if (down == true)
				this->doDropFlag();
			break;
	}

	battlefield->followCameraController->onKey(key, down);

	if (this->localUserData == nullptr)
		return;

	if (c == true)
		applyControlState(this->localUserData, this->controlBits);

	WeaponCommonModel *weaponCommonModel = Service::getWeaponCommonModel();
	if (weaponCommonModel != nullptr)
		weaponCommonModel->onKey(key, down);
}


static void interpolateVector3d(double smooth, Vector3 *in1, Vector3 *in2, Vector3 *out)
{
	_function_name("TankModel::interpolateVector3d");

	//smooth = 1;
	out->x = in1->x + ((in2->x - in1->x) * smooth);
	out->y = in1->y + ((in2->y - in1->y) * smooth);
	out->z = in1->z + ((in2->z - in1->z) * smooth);
}


static void interpolateOrientation(double smooth, Quaternion *param2, Vector3 *param3, Vector3 *param4)
{
	_function_name("TankModel::interpolateOrientation");

	smooth = 1;
	Quaternion orientation;
	Vector3 eulerAngles;
	orientation.setFromEulerAnglesXYZ(param3->x, param3->y, param3->z);
	orientation.slerp(param2, &orientation, smooth);
	orientation.getEulerAngles(&eulerAngles);
	param4->x = eulerAngles.x;
	param4->y = eulerAngles.y;
	param4->z = eulerAngles.z;
}

void TankModel::move(ClientObjectUser *clientObject, Vector3 *pos, Vector3 *orientation, Vector3 *linearVelocity,
					 Vector3 *angleVelocity, double turretAngle, int control, bool isControl)
{
	_function_name("TankModel::move");

	TankData *tankData = this->getTankData(clientObject);
	if (tankData == nullptr)
		return;
	Tank *tank = tankData->tank;
	if (tank == nullptr)
		return;
	if (tankData == this->localUserData)
		return;

	interpolateVector3d(0.5, &tank->state.pos, pos, pos);
	interpolateVector3d(0.5, &tank->state.velocity, linearVelocity, linearVelocity);
	interpolateVector3d(0.5, &tank->state.rotation, angleVelocity, angleVelocity);
	interpolateOrientation(0.5, &tank->state.orientation, orientation, orientation);
	setTankState(tankData, pos, orientation, linearVelocity, angleVelocity, turretAngle, control, isControl);
}



void TankModel::kill(ClientObjectUser *clientObject, DeathReason reason, const QString &killerTankId)
{
	_function_name("TankModel::kill");
	(void)reason;
	(void)killerTankId;

	if (clientObject == nullptr)
		return;

	TankData *tankData = this->getTankData(clientObject);

	if (tankData == nullptr || tankData->tank == nullptr)
		return;

	this->setControlState(tankData, 0, true);
	//tankData->logEvent("Killed");
	tankData->enabled = false;
	tankData->health = 0;
	tankData->deadTime = RESPAWN_DELAY;
	tankData->weapon->ownerDisabled(clientObject);
#ifdef GRAPHICS
	tankData->tank->title()->hideIndicators();
#endif
	tankData->spawnState = TankSpawnState::SUICIDE;
	if (tankData->local == true)
	{
		this->_userControlsEnabled = false;
		this->awaitingKillCommand = false;
		this->readyToSpawnCommandSent = false;
		this->battlefield->hideSuicideIndicator();
		this->suicideTime = INT_MAX;
		tankData->tank->setCollisionGroup(tankData->tank->getCollisionGroup() & ~CollisionGroup::BONUS_WITH_TANK);
	}
	this->createDeathEffects(tankData);
	//this->showUserDeathMessage(clientObject, reason, killerTankId);
	this->tankDispatcher->dispatchEvent(TankEvent::KILLED, tankData);
	this->stop(tankData);
	//this->battlefield.logicUnits.addLogicUnit(new BeforeKillTankTask((getTimer() + 3000), tankData.tank));
}


void TankModel::createDeathEffects(TankData *tankData)
{
	_function_name("TankModel::createDeathEffects");
	TankDeathGraphicEffect *tankDeathGraphicEffect = Service::getIBattleField()->getObjectPoolService()->getTankDeathGraphicEffect();
	tankDeathGraphicEffect->init(tankData);
	this->battlefield->addGraphicEffect(tankDeathGraphicEffect);
}


void TankModel::objectUnloaded(ClientObjectUser *clientObject)
{
	_function_name("TankModel::objectUnloaded");

	TankData *tankData = this->getTankData(clientObject);
	if (tankData == nullptr)
		return;

	Service::getIMain()->deleteTank(tankData->tank);

	if (tankData->tank != nullptr)
		TANKS::tank_delete(tankData->tank);

	this->battlefield->removeTank(tankData);
	if (tankData->tank != nullptr)
	{
		if (tankData->weapon != nullptr)
			tankData->weapon->ownerUnloaded(clientObject);
		if (tankData->tank->skin != nullptr)
			tankData->tank->skin->dispose();
	}

	if (tankData == this->localUserData)
	{
		TankData::localTankData = nullptr;
		this->localUserData = nullptr;
#ifdef GRAPHICS
		this->userTitlesRender->setLocalData(nullptr);
#endif
		this->battlefield->setLocalUser(nullptr);
		//Main.stage.removeEventListener(KeyboardEvent.KEY_DOWN, this->onKey);
		//Main.stage.removeEventListener(KeyboardEvent.KEY_UP, this->onKey);
		//this->resume();
		//ILoaderWindowService(Main.osgi.getService(ILoaderWindowService)).unlockLoaderWindow();
	}

	this->tankDispatcher->dispatchEvent(TankEvent::UNLOADED, tankData);
	this->tankDataById->remove(clientObject->getId());
	this->numTanks--;

	Tank *t = tankData->tank;
	if (t != nullptr)
		t->destroy();
	else
		delete tankData;

	/*
	if (this->numTanks == 0)
	{
		dumpService = IDumpService(Main.osgi.getService(IDumpService));
		if (dumpService != null)
		{
			dumpService.unregisterDumper(this->dumperName);
		}
	}
	*/

	//StatisticsModel(Main.osgi.getService(IBattlefieldGUI)).userDisconnect(null, tankData.userName);
}


void TankModel::changeSpecification(ClientObjectUser *clientObject, const TankSpecification &tankSpecification, bool immediate)
{
	TankData *tankData = this->getTankData(clientObject);
	if (tankData == nullptr)
		return;
	if (tankData->tank == nullptr)
		return;

	this->setTankSpec(tankData, &tankSpecification, immediate);
}


void TankModel::startSuicideCountdown()
{
	if (this->_userControlsEnabled && !this->awaitingKillCommand && this->suicideTime == INT_MAX)
	{
		this->suicideTime = GetGameTime() + SUICIDE_DELAY;
		this->battlefield->showSuicideIndicator(SUICIDE_DELAY);
	}
}

void TankModel::doDropFlag()
{
	CTFModel *ctf = Service::getCTFModel();
	if (ctf != nullptr)
		ctf->doDropFlag();
}

#ifdef L_OT
void TankModel::moveCorrectCommand(ClientObjectUser *obj, int time, Vector3 *pos, Vector3 *orientation, Vector3 *linVel, Vector3 *angVel, double turretDir, int controlBits)
{
	(void)obj;
	(void)time;


	this->possz = pos->z;
	this->orientations = orientation->z;
	this->linVels = linVel->z;
	this->angVels = angVel->z;

	if (this->controlsLocked)
	{
		controlBits = 0;
		int turretRotationDir = (((this->localUserData->ctrlBits & TankModel::TURRET_LEFT) || (this->localUserData->ctrlBits & TankModel::LEFT)) >> 4) - (((this->localUserData->ctrlBits & TankModel::TURRET_RIGHT) || (this->localUserData->ctrlBits & TankModel::RIGHT)) >> 5);
		int turretRotationDirAddition = (((this->localUserData->ctrlBits & TankModel::LEFT) >> 2) - ((this->localUserData->ctrlBits & TankModel::RIGHT) >> 3));
		if (turretRotationDir == 0)
		{
			turretRotationDir = turretRotationDirAddition;
		}
		if (turretRotationDir == 1)
		{
			controlBits = TankModel::TURRET_LEFT;
		}
		else
		{
			if (turretRotationDir == -1)
			{
				controlBits = TankModel::TURRET_RIGHT;
			}
			else
			{
				if (((turretRotationDir == 0) && (!((this->localUserData->ctrlBits & TankModel::CENTER_TURRET) == 0))))
				{
					controlBits = TankModel::CENTER_TURRET;
				}
			}
		}
	}

	QString s;
	s.append("battle;move_correct;");
	s.append(double_to_string(pos->x) + QChar('@'));
	s.append(double_to_string(pos->y) + QChar('@'));
	s.append(double_to_string(pos->z) + QChar('@'));

	s.append(double_to_string(orientation->x) + QChar('@'));
	s.append(double_to_string(orientation->y) + QChar('@'));
	s.append(double_to_string(orientation->z) + QChar('@'));

	s.append(double_to_string(linVel->x) + QChar('@'));
	s.append(double_to_string(linVel->y) + QChar('@'));
	s.append(double_to_string(linVel->z) + QChar('@'));

	s.append(double_to_string(angVel->x) + QChar('@'));
	s.append(double_to_string(angVel->y) + QChar('@'));
	s.append(double_to_string(angVel->z) + QChar('@'));

	s.append(QChar(';') + QString::number(turretDir) + QChar(';') + QString::number(controlBits) + ";1");

	Service::getNetworker()->send(s);

	//Network(Main.osgi.getService(INetworker)).send(((((((((((("battle;move_correct;" + ((((pos.x + "@") + pos.y) + "@") + this->possz.value)) + "@") + ((((orientation.x + "@") + orientation.y) + "@") + this->orientations.value)) + "@") + ((((linVel.x + "@") + linVel.y) + "@") + this->linVels.value)) + "@") + ((((angVel.x + "@") + angVel.y) + "@") + this->angVels.value)) + ";") + turretDir) + ";") + controlBits));

	//battle;move_correct;1228.9070146921931@4329.109011503629@-212.68122100830078@-3.272607800263706e-17@1.5614303406536308e-16@2.145137575108865@0.000004684694665274335@0.000002669900099047001@-1.846927663935337e-15@-9.820365744711518e-18@-3.302099149333922e-17@3.41758789810777e-13;0;0

	//battle;move;        1228.907015801447@4329.109012135814@-212.68122100830078@-4.1834630623739725e-17@1.6862818994728552e-16@2.1451375751089454@0.000004573435479603632@0.0000026064912897079396@-1.803064043664557e-15@-5.3986193944662495e-17@-1.107438357150559e-16@3.3364218726418913e-13;0;1
}
#endif


void TankModel::moveAnticheat(ClientObjectUser *clientObject, Vector3 *pos, Vector3 *orientation, Vector3 *linearVelocity, Vector3 *angleVelocity, double turretAngle, int control, bool isControl)
{
	(void)turretAngle;
	(void)control;
	(void)isControl;
	static const Vector3 zeroVector3d(0, 0, 0);
	TankData *tankData = this->getTankData(clientObject);
	Tank *tank = tankData->tank;
	interpolateVector3d(0.5, &tank->state.pos, pos, pos);
	interpolateVector3d(0.5, &tank->state.velocity, linearVelocity, linearVelocity);
	interpolateVector3d(0.5, &tank->state.rotation, angleVelocity, angleVelocity);
	interpolateOrientation(0.5, &tank->state.orientation, orientation, orientation);
	this->setTankState(tankData, pos, orientation, &zeroVector3d, &zeroVector3d, 0, 0, true);
	this->moveCommand(this->localUserData->user, 0, pos, orientation, linearVelocity, angleVelocity, this->localUserData->tank->getTurretDir(), this->localUserData->ctrlBits);
	Service::getNetworker()->send("battle;ok");
}

void TankModel::mandatoryUpdate()
{
	Vector3 pos, orient, linVel, angVel;
	this->localUserData->tank->getPhysicsState(&pos, &orient, &linVel, &angVel);
	this->moveCommand(this->localUserData->user, this->battlefield->bfData->physTime, &pos, &orient, &linVel, &angVel, this->localUserData->tank->getTurretDir(), this->localUserData->ctrlBits);
}

void TankModel::setTemperature(ClientObjectUser *clientObject, double value)
{
	if (clientObject == nullptr)
		return;

	TankData *tankData = this->getTankData(clientObject);
	if (tankData == nullptr || tankData->tank == nullptr)
	{
		//Logger.log(LogLevel.LOG_ERROR, ((("TankModel: Incorrect call of setTemperature() clientObject=" + clientObject) + ", tankData=") + tankData));
		return;
	}
	this->setTankTemperature(tankData->tank, value);
}

void TankModel::changeHealth(ClientObjectUser *clientObject, int newHealth)
{
	if (clientObject == nullptr)
		return;

	TankData *tankData = this->getTankData(clientObject);
	if (tankData == nullptr || tankData->tank == nullptr)
	{
		//Logger.log(LogLevel.LOG_ERROR, ((("TankModel: Incorrect call of changeHealth() clientObject=" + clientObject) + ", tankData=") + tankData));
		return;
	}

	this->setTankHealth(tankData, newHealth);
}



void TankModel::setTankTemperature(Tank *tank, double temperature)
{
	if (tank == nullptr || tank->skin == nullptr)
		return;
	tank->skin->targetColorTransformOffset = -temperature;
}

void TankModel::createCriticalHitEffect(const QString &userId)
{
#ifdef GRAPHICS
	TankData *tankData = this->tankDataById->value(userId);
	if (tankData != nullptr)
	{
		this->battlefield->addGraphicEffect(new TankCriticalHitGraphicEffect(tankData));
	}
#else
	(void)userId;
#endif
}

void TankModel::createDamageEffect(Tank *tank, int damage, uint32_t color)
{
#ifdef GRAPHICS
	damageEffect->createEffect(tank, damage, color);
#endif
	Service::getIMain()->createDamageEffect(tank, damage, color);
}

void TankModel::lockControls(bool lock)
{
	this->controlsLocked = lock;
}




void TankModel::initModel()
{
	if (this->effectsModel != nullptr)
		return;
	//this->modelService = IModelService(Main.osgi.getService(IModelService));
	this->tankDispatcher = battlefield->getTankEventDispatcher();
	//this->battlefield = (Main.osgi.getService(IBattleField) as BattlefieldModel);
	//this->userTitlesRender = (((Main.osgi.getService(IBattleField) as BattlefieldModel).spectatorMode) ? new SpectatorUserTitlesRender() : new DefaultUserTitlesRender());
	//this->userTitlesRender.setBattlefield(this.battlefield);
	//this->gui = (Main.osgi.getService(IBattlefieldGUI) as IBattlefieldGUI);
	this->effectsModel = Service::getBattleController()->effectsModel;
	//this->inventoryModel = IInventory(this.modelService.getModelsByInterface(IInventory)[0]);
	//var locale:ILocaleService = ILocaleService(Main.osgi.getService(ILocaleService));
	//this->suicideMessage = locale.getText(TextConst.BATTLE_PLAYER_SUICIDED);
	//this->killMessage = locale.getText(TextConst.BATTLE_PLAYER_KILLED);
	//this->initKeyTable();
	/*
	var networkService:INetworkService = INetworkService(Main.osgi.getService(INetworkService));
	if (networkService != null)
	{
		networkService.addEventListener(this);
	}
	var focusService:IFocusService = IFocusService(Main.osgi.getService(IFocusService));
	if (focusService != null)
	{
		focusService.addFocusListener(this);
	}
	*/
}


void TankModel::createLevelUpEffect(const QString &userId, int rang)
{
#ifdef GRAPHICS
	TankData *tankData = this->tankDataById->value(userId);
	if (tankData != nullptr)
	{
		LevelUpEffect::createEffect(battlefield, tankData->tank, rang);
		tankData->tank->title()->setRank(rang);
		//Sound levelUpSound = ResourceUtil::getResource(ResourceType.SOUND, "levelUpSound").sound;
		//this->battlefield->soundManager->playSound(levelUpSound, 0, 1);
	}
#else
	(void)userId;
	(void)rang;
#endif
}

void TankModel::movementControl(ClientObjectUser *user, int control)
{
	TankData *tankData = user->tankData;
	if (tankData == nullptr)
		return;

	//tankData->_SafeStr_1218(control);

	this->applyControlState(tankData, control);
}

void TankModel::rotateTurret(ClientObjectUser *user, RotateTurretCommand *rotateTurretCommand)
{
	(void)user;
	(void)rotateTurretCommand;
}

