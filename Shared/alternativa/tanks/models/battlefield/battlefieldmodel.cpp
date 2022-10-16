#include "battlefieldmodel.h"
#include "battlefielddata.h"
#include "alternativa/tanks/camera/ICameraController.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/models/tank/ITank.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/config/config.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "service.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/tanks/camera/followcameracontroller.h"
#include "alternativa/tanks/camera/flycameracontroller.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/math/quaternion.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "networking/network.h"
#include "IMain.h"
#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "gametime.h"
#include "IBattlefieldPlugin.h"
#include "alternativa/tanks/models/effects/common/bonuscommon/bonuscommonmodel.h"
#include "alternativa/tanks/models/effects/common/bonuscommon/BonusCommonData.h"
#include "alternativa/tanks/bonuses/IBonus.h"
#include "alternativa/tanks/battle/triggers/triggers.h"
#include "double_to_string.h"
#include "alternativa/engine3d/core/view.h"
#include "alternativa/engine3d/containers/kdcontainer.h"
#include "resource/resourceutil.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/engine3d/objects/skybox.h"
#include "alternativa/tanks/models/battlefield/skybox/object3drevolver.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/config/tanksmap.h"
#include "alternativa/object/clientobjectuser.h"
#include "decals/fadingdecalsrenderer.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "_global.h"
#include "tanks.h"
#include "alternativa/engine3d/materials/textureresourcesregistry.h"
#include "alternativa/gfx/core/bitmaptextureresource.h"
#include "__global.h"
#include "flash/geom/matrix.h"
#include "alternativa/resource/stubbitmapdata.h"
#include "alternativa/tanks/display/usertitle/effectindicator.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/tank/tankeventdispatcher.h"
#include "hidableobjects/hidablegraphicobjects.h"
#include "logic/battlelogicunits.h"
#include "objects2tank.h"
#include "flash/display/stage.h"
#include "gamemode/IGameMode.h"
#include "gamemode/gamemodes.h"
#include "alternativa/tanks/model/panel/panelmodel.h"
#include "dust/dust.h"

#include <QMutableHashIterator>
#include "mymath.h"
#include "gametime.h"
#include <QByteArray>
#include <QTimer>
#include <QHash>
#include <QDateTime>



#define PHYSICS_STEP_MILLIS 30
#define MAX_FRAME_TIME 200

static const int DECAL_FADING_TIME_MS = 20000;
static const int CAMERA_FLY_TIME = 3000;


class BattlefieldLoop : public QObject
{
	BattlefieldModel *d;
public:
	BattlefieldLoop(BattlefieldModel *d)
	{
		this->d = d;
	}

	~BattlefieldLoop()
	{

	}

protected:
	void timerEvent(QTimerEvent *event) override
	{
		(void)event;
		d->loop();
	}
};


BattlefieldModel::BattlefieldModel()
{
	screenSize = screenSizeSteps = 10;

	lastLogicUnitsUpdate = 0;
	deltaSec = 0;
	activeCameraController = nullptr;
	mapResourceId = new QString();
	doRender = true;
	object = new QObject;
	object_loop = new BattlefieldLoop(this);
	timer_loop = new QTimer;
	bfData = nullptr;
	//QObject::connect(timer_loop, QTimer::timeout, object, [this] { loop(); } );
	plugins = new QVector<IBattlefieldPlugin*>;
	pluginCount = 0;
	triggers = new Triggers;
	animatedTracks = true;
	config = nullptr;
	followCameraController = nullptr;
	objectPoolService = new ObjectPoolService();
	tankEventDispatcher = new TankEventDispatcher();
	fps_tfDelay = 0;
	fps_tfTimer = 0;
	fadingDecalRenderer = nullptr;
	hidableObjects = new HidableGraphicObjects();
	logicUnits = new BattleLogicUnits();
	objects2Tank = new Objects2Tank;
	_gameMode = nullptr;
	dusts = nullptr;
	loop_id = 0;
}

BattlefieldModel::~BattlefieldModel()
{	
	this->objectUnloaded(nullptr);

	delete mapResourceId;

	stopLoop();
	object_loop->deleteLater();

	timer_loop->deleteLater();
	timer_loop = nullptr;
	delete object;

	delete triggers;

	pluginCount = 0;

	delete plugins;
	delete objectPoolService;
	delete tankEventDispatcher;
	delete hidableObjects;
	delete logicUnits;
	delete objects2Tank;
}


void BattlefieldModel::objectUnloaded(ClientObject*)
{
	stopLoop();

	TANKS::clear();

	hideSuicideIndicator();

	if (this->config != nullptr)
	{
		delete this->config;
		this->config = nullptr;
	}

	if (this->bfData != nullptr)
	{
		BattlefieldData *bfData = this->bfData;

		if (bfData->tanks != nullptr)
		{
			for (int j = 0; j < 100; j++)
			{
				if (bfData->tanks->count() == 0)
					break;
				auto i = bfData->tanks->begin();
				Tank *tank = i.key();
				ClientObjectUser *user = tank->tankData->user;
				if (user != nullptr) {
					this->tankInterface->objectUnloaded(user);
				}
			}

			delete bfData->tanks;
			bfData->tanks = nullptr;
		}

		if (bfData->activeTanks != nullptr)
		{
			delete bfData->activeTanks;
			bfData->activeTanks = nullptr;
		}

		if (bfData->graphicEffects != nullptr)
		{
			QHashIterator<ISpecialEffect*, bool> i(*bfData->graphicEffects);
			while (i.hasNext()) {
				i.next();
				i.key()->destroy();
			}
			delete bfData->graphicEffects;
			bfData->graphicEffects = nullptr;
		}

		if (bfData->viewport != nullptr)
		{
			bfData->viewport->clearContainers();
			delete bfData->viewport;
			bfData->viewport = nullptr;
		}

		if (bfData->bonuses != nullptr)
		{
			{
				QHashIterator<QString, IBonus*> i(*bfData->bonuses);
				while (i.hasNext()) {
					i.next();
					i.value()->destroy();
				}
			}

			bfData->bonuses->clear();
			delete bfData->bonuses;
			bfData->bonuses = nullptr;
		}

		if (bfData->collisionDetector != nullptr)
		{
			bfData->collisionDetector->destroy();
			bfData->collisionDetector = nullptr;
		}

		if (bfData->physicsScene != nullptr)
		{
			delete bfData->physicsScene;
			bfData->physicsScene = nullptr;
		}


		delete bfData;
		this->bfData = nullptr;
	}

	Service::setBattlefieldModel(nullptr);
	Service::setIBattleField(nullptr);

	if (followCameraController != nullptr)
	{
		delete followCameraController;
		followCameraController = nullptr;
	}

	if (flyCameraController != nullptr)
	{
		delete flyCameraController;
		flyCameraController = nullptr;
	}

	if (fadingDecalRenderer != nullptr)
	{
		delete fadingDecalRenderer;
		fadingDecalRenderer = nullptr;
	}



	Service::getIMain()->clearFps();
#ifdef GRAPHICS
	TextureResourcesRegistry::clear();
	BitmapTextureResource::clear();
	EffectIndicator::_clear();
#endif

	ResourceUtil::clear();
	TankSkin::_clear();



#ifdef GRAPHICS
	delete dusts;
#endif


	//ResourceLoaderListener.clearListeners(true);
	//ResourceUtil.loadImages();


	/*
	var i:int;
	while (i < this->toDestroy.length)
	{
		o = this->toDestroy[i];
		if (o != null)
		{
			if (this->blacklist.indexOf(o) == -1)
			{
				o.destroy(true);
				o = null;
				i++;
			}
			else
			{
				this->toDestroy.removeAt(i);
			}
		}
		if (i >= 200) break;
		i++;
	}
	*/


	//if (i < 200)
	//this->toDestroy = new Vector.<Object>();

	//Main.debug.unregisterCommand(CONSOLE_COMMAND_ID);
	//this->materialRegistry.clear();
	//this->removeMainListeners();
	//this->removeKeyboardListeners();
	//this->_soundManager.stopAllSounds();
	//this->_soundManager.removeAllEffects();
	//this->bfData->viewport->camera->view->clear();
	//this->bfData->viewport->camera->view = nullptr;

	//this->bfData->guiContainer.removeChild(this->bfData.viewport);
	//IFocusService(Main.osgi.getService(IFocusService)).clearFocus(this->bfData.viewport);
	//this->bfData->guiContainer.stage.frameRate = MAX_FRAMERATE;
	//if (this->freeCameraController != nullptr)
	//this->freeCameraController.deactivate();

	//Main.contentUILayer.visible = true;
	//this->setCameraTarget(nullptr);
	//this->debugPanel = null;




	/*
	if (this->bfData->physicsScene->_staticCD != nullptr)
	{
		delete this->bfData->physicsScene->_staticCD;
		this->bfData->physicsScene->_staticCD = nullptr;
	}
	*/

	/*
	if (this->bfData->physicsScene != nullptr)
	{
		delete this->bfData->physicsScene;
		this->bfData->physicsScene = nullptr;
	}
	*/

	//this->suicideIndicator = nullptr;


	/*
	IBackgroundService *bgService = IBackgroundService(Main.osgi.getService(IBackgroundService));
	if (bgService != null)
	{
		bgService.drawBg();
	}
	*/
	//var dumpService:IDumpService = IDumpService(Main.osgi.getService(IDumpService));
	//if (dumpService != null)
	//dumpService.unregisterDumper(this->dumperName);
	//var storageService:IStorageService = IStorageService(Main.osgi.getService(IStorageService));
	//storageService.getStorage().data.cameraHeight = this->followCameraController.cameraHeight;
	//var tankEventDispatcher:ITankEventDispatcher = ITankEventDispatcher(Main.osgi.getService(ITankEventDispatcher));
	//tankEventDispatcher.removeTankEventListener(TankEvent.KILLED, this);
	//tankEventDispatcher.removeTankEventListener(TankEvent.SPAWNED, this);
	//Main.contentUILayer.removeChild(this->messages);
	//Main.stage.removeEventListener(Event.RESIZE, this->onStageResize);
	//this->messages = null;
}

void BattlefieldModel::resetFollowCamera()
{
	if (((!(this->bfData == nullptr)) && (!(this->bfData->viewport == nullptr))))
	{
		this->bfData->viewport->camera->rotation.y = 0;
		this->followCameraController->initCameraComponents();
	}
}

void BattlefieldModel::activateFollowCamera()
{
	this->followCameraController->activate();
	this->followCameraController->setLocked(false);
	this->activeCameraController = this->followCameraController;
	this->tankInterface->lockControls(false);
}

HidableGraphicObjects *BattlefieldModel::getHidableGraphicObjects()
{
	return this->hidableObjects;
}

BattleLogicUnits *BattlefieldModel::getBattleLogicUnits()
{
	return this->logicUnits;
}

void BattlefieldModel::addDecal(const Vector3 *hitPosition, const Vector3 *muzzlePosition, double radius, TextureMaterial *material, RotationState rotationState)
{
	this->bfData->viewport->addDecal(hitPosition, muzzlePosition, radius, material, rotationState);
}



QString BattlefieldModel::getMapResourceId()
{
	return *mapResourceId;
}

void BattlefieldModel::initObject(ClientObject *clientObject, BattlefieldResources *battlefieldResources, BattlefieldSoundScheme *battlefieldSoundScheme, int idleKickPeriodMsec, const QString &mapDescriptorResourceId, int respawnInvulnerabilityPeriodMsec, const QString &skyboxTextureResourceId, bool spectator, IGameMode *gameMode)
{
	(void)battlefieldResources;
	(void)battlefieldSoundScheme;
	(void)spectator;
	//this->debugMode = (!(Main.osgi.getService(IDebugService) == null));
	//this->spectatorMode = spectator;
	this->_gameMode = gameMode;
	//this->panelUnlockCounter = 2;
	//this->throwDebugError = false;
	this->tankInterface = Service::getTankModel();
	//INetworkService (networkService = (Main.osgi.getService(INetworkService) as INetworkService);
	//if (networkService != null)
	//{
	//networkService.addEventListener(this);
	//}
	//IModelService *modelService = IModelService(Main.osgi.getService(IModelService));
	//this->gui = (Main.osgi.getService(IBattlefieldGUI) as IBattlefieldGUI);
	this->bfData = new BattlefieldData();
	this->bfData->bfObject = clientObject;
	this->bfData->tanks = new QHash<Tank*, TankData*>;
	this->bfData->activeTanks = new QHash<TankData*, bool>;
	this->bfData->graphicEffects = new QHash<ISpecialEffect*, bool>;
	this->bfData->bonuses = new QHash<QString, IBonus*>;
	this->bfData->physicsScene = nullptr;
	this->bfData->collisionDetector = nullptr;
	this->bfData->skybox = nullptr;
	this->bfData->localUser = nullptr;



	//this->bfData->guiContainer = (Main.osgi.getService(IMainContainerService) as IMainContainerService).contentLayer;
	*this->mapResourceId = mapDescriptorResourceId;
	this->bfData->respawnInvulnerabilityPeriod = respawnInvulnerabilityPeriodMsec;
	this->bfData->idleKickPeriod = idleKickPeriodMsec;
	//this->initSounds(battlefieldSoundScheme);
	this->initPhysicsAndViewport();
	this->initMap(mapDescriptorResourceId, skyboxTextureResourceId);
	//IDumpService *dumpService = IDumpService(Main.osgi.getService(IDumpService));
	//if (dumpService != null)
	//{
	//dumpService.registerDumper(this);
	//}
	this->systemTimeStart = QDateTime::currentMSecsSinceEpoch();
	SetGameTime(0);
	//this->logicTime = 0;
	this->physicsTime = 0;
	//this->a3dRenderTime = 0;
	this->bfData->physTime = 0;
	this->bfData->time = 0;

}

void BattlefieldModel::removeTankFromField(TankData *tankData)
{
	if (this->bfData->activeTanks->contains(tankData) == false)
		return;

	this->bfData->activeTanks->remove(tankData);
	//tankData->logEvent("Removed from field");
	tankData->tank->removeFromContainer();
	this->bfData->physicsScene->removeBody(tankData->tank);
	this->bfData->collisionDetector->removeBody(tankData->tank);
	this->tankInterface->stop(tankData);
	//this->_soundManager.removeEffect(tankData.sounds);


	for (int i = 0; i < this->pluginCount; i++)
		this->plugins->at(i)->removeUserFromField(tankData->user);


	this->bfData->time = GetGameTime();

	if (this->followCameraController->getTank() == tankData->tank)
		this->followCameraController->deactivate();
}

void BattlefieldModel::setLocalUser(ClientObjectUser *clinetObject)
{
	if (this->bfData == nullptr)
		return;
	this->bfData->localUser = clinetObject;
}

void BattlefieldModel::setCameraTarget(Tank *tank)
{
	this->followCameraController->setTank(tank);
}

BattlefieldData *BattlefieldModel::getBattlefieldData()
{
	return bfData;
}

void BattlefieldModel::addTank(TankData *tankData)
{
	if (this->bfData->tanks->contains(tankData->tank))
		return;


	this->bfData->tanks->insert(tankData->tank, tankData);
	if (tankData->enabled == true)
	{
		//(Main.osgi.getService(IConsole) as IConsole).addLine("call addTankToField");
		this->addTankToField(tankData);
	}

	for (int i = 0; i < this->pluginCount; i++)
		this->plugins->at(i)->addUser(tankData->user);

	if (tankData->local == true)
	{
		this->updatePanelUnlockCounter();
	}
}



void BattlefieldModel::initPhysicsAndViewport()
{
	PhysicsScene *physics = new PhysicsScene();
	this->bfData->physicsScene = physics;
	physics->usePrediction = true;

	if (*this->mapResourceId == "map_silence_space")
	{
		Vector3 gravity(0, 0, -450);
		physics->setGravity(&gravity);
	}
	else
	{
		Vector3 gravity(physics->realX, physics->realY, physics->realZ);
		physics->setGravity(&gravity);
	}

	physics->collisionIterations = 3;
	physics->contactIterations = 3;
	physics->maxPenResolutionSpeed = 100;
	physics->allowedPenetration = 5;
	physics->collisionDetector = this->bfData->collisionDetector = new TanksCollisionDetector();

#ifdef GRAPHICS
	this->fadingDecalRenderer = new FadingDecalsRenderer(DECAL_FADING_TIME_MS, this);
#else
	this->fadingDecalRenderer = nullptr;
#endif
	this->bfData->viewport = new BattleView3D(false, this->bfData->collisionDetector, this);

	int width, height;
	Service::getIMain()->getSize(&width, &height);
	this->bfData->viewport->camera->setViewSize(width, height);

	this->bfData->viewport->camera->softTransparency = true;
	this->bfData->viewport->camera->softTransparencyStrength = 1;

	//this->shadows = new BattleShadow(this->bfData.viewport);
	//this->shadows.on();
	//this->bfData->guiContainer.addChild(this->bfData.viewport);

#ifdef GRAPHICS
	this->dusts = new Dust(this);

	std::shared_ptr<BitmapData> image = ResourceUtil::getResourceImageLocal("dust/dust_data.png");

	if (image == nullptr)
	{
		qDebug("error load dust");
		return;
	}
	if (0) // if (this.mapResourceId.indexOf("_winter") != -1)
	{
		//this.dusts.init(new SNOW_DUST_DATA().bitmapData, 7000, 5000, 180, 0.75, 0.15);
	}
	else
	{
		this->dusts->init(image, 7000, 5000, 180, 0.75, 0.15);
	}
#endif

	//this->suicideIndicator = new SuicideIndicator();
	//this->bfData->viewport->addChild(this->suicideIndicator);
	//Main.stage.focus = this->bfData.viewport;
	//this->cameraUnlockCounter = 0;
	this->doRender = false;
	//this->debugPanel = new DebugPanel();
	//this->debugPanel.visible = false;

	this->followCameraController = new FollowCameraController(this->bfData->collisionDetector, this->bfData->viewport->camera, CollisionGroup::CAMERA);
	this->flyCameraController = new FlyCameraController(this->bfData->viewport->camera);
	//this->freeCameraController = new SpectatorCameraController(this->bfData.viewport.camera);
	//IBattleSettings *settings = this->getBattleSettings();

	//FollowCameraController(this->followCameraController).setDefaultSettings();
	//var storage:IStorageService = (Main.osgi.getService(IStorageService) as IStorageService);
	//this->screenSize = storage.getStorage().data.screenSize;
	//if (this->screenSize == 0)
	//{
	//this->screenSize = this->screenSizeSteps;
	//}

	//int cameraHeight = storage.getStorage().data.cameraHeight;
	//if (cameraHeight != 0)
	//{
	//this->followCameraController->cameraHeight = cameraHeight;
	//}
	//this->decalFactory = new DecalFactory(this->bfData.collisionDetector);

	//Stage *stage = this->bfData->viewport->getStage();
	//double w = stage->stageWidth() * this->screenSize;
	//double h = stage->stageHeight() * this->screenSize;
	//this->bfData->viewport->resize(w, h);
}


void BattlefieldModel::loop()
{
	int32_t t1 = QDateTime::currentMSecsSinceEpoch() - systemTimeStart;

	SetGameTime(t1);

	int32_t deltaMsec = t1 - this->bfData->time;

	if (deltaMsec < 1)
		return;

	this->bfData->time = t1;

	this->deltaSec = double(deltaMsec) * 0.001;

	if (++this->fps_tfDelay >= 10)
	{
		uint32_t last = this->fps_tfTimer;
		this->fps_tfTimer = t1;
		uint32_t delta = this->fps_tfTimer - last;
		uint32_t fps = (1000 * 10) / delta;
		this->fps_tfDelay = 0;
		Service::getIMain()->setFps(fps);
	}

	/*
	if (this->adaptiveFpsEnabled)
	{
		if (++this->adaptiveFrameCounter == ADAPTIVE_FPS_CHANGE_INTERVAL)
		{
			this->adaptiveFrameCounter = 0;
			if (this->fullTimeStat.avrgFps < (Main.stage.frameRate - 1))
			{
				Main.stage.frameRate = ((this->fullTimeStat.avrgFps < MIN_FRAMERATE) ? Number(MIN_FRAMERATE) : Number(this->fullTimeStat.avrgFps));
			}
			else
			{
				fps = (Main.stage.frameRate + 1);
				Main.stage.frameRate = ((fps > MAX_FRAMERATE) ? Number(MAX_FRAMERATE) : Number(fps));
			}
			if (this->debugPanel.visible)
			{
				this->debugPanel.printValue("Stage frame rate", Main.stage.frameRate.toFixed(2));
			}
		}
	}
	*/

	/*
	{
		QHashIterator<Tank*, TankData*> i(*this->bfData->tanks);
		while (i.hasNext()) {
			i.next();
			Tank *tank = i.key();
			tank->new_contact_normal = false;
			tank->normal_contact_new = false;
		}
	}
	*/

	//t1 = getTimer();
	this->runPhysics(PHYSICS_STEP_MILLIS);


	//uint32_t t2 = getTimer();
	//this->physicsTime = (this->physicsTime + (t2 - t1));
	//t1 = t2;
	if (this->activeCameraController != nullptr)
	{
		this->activeCameraController->update(this->bfData->time, deltaMsec);
	}


	this->bfData->viewport->camera->calculateAdditionalData();

#ifdef GRAPHICS
	if (this->skyboxRevolver != nullptr)
	{
		this->skyboxRevolver->render(this->bfData->physTime, deltaMsec);
	}
#endif


	double t = 1. - ((this->bfData->physTime - this->bfData->time) / double(PHYSICS_STEP_MILLIS));
	this->updateTanks(this->bfData->time, deltaMsec, this->deltaSec, t);


	for (int i = 0; i < this->pluginCount; i++)
		this->plugins->at(i)->tick(this->bfData->time, deltaMsec, this->deltaSec, t);


	this->lastLogicUnitsUpdate = (int32_t)this->bfData->physTime - this->lastLogicUnitsUpdate;
	this->logicUnits->update(this->bfData->physTime, this->lastLogicUnitsUpdate);
#ifdef GRAPHICS
	this->fadingDecalRenderer->render(GetGameTime());
#endif


	if (this->hidableObjects->isEnabled())
	{
		this->hidableObjects->render(GetGameTime(), deltaMsec);
	}


	this->playSpecialEffects(deltaMsec);
	this->updateBonuses(this->bfData->time, deltaMsec, t);
	//deltaMsec = (getTimer() - this->bfData->time);
	//t2 = getTimer();
	//this->logicTime = (this->logicTime + (t2 - t1));
	//t1 = t2;
	if (this->doRender && Service::getIMain()->renderEnable())
	{
		this->bfData->viewport->update();
	}

#ifdef GRAPHICS
	this->dusts->update();
#endif
	/*
	if (this->debugPanel->visible)
	{
		runningTime = (getTimer() - this->startTime);
		this->debugPanel.printValue("Running time", runningTime);
		this->debugPanel.printValue("Physics time", this->physicsTime, ":", Number((this->physicsTime / runningTime)).toFixed(4));
		this->debugPanel.printValue("Logic time", this->logicTime, ":", Number((this->logicTime / runningTime)).toFixed(4));
		this->debugPanel.printValue("A3D render time", this->a3dRenderTime, ":", Number((this->a3dRenderTime / runningTime)).toFixed(4));
	}
	*/

	//this->messages->update((deltaMsec * 10));
}


void BattlefieldModel::updateTanks(uint32_t time, uint32_t deltaMillis, double deltaSec, double t)
{
	//time, deltaMillis, deltaSec, t

	const Vector3 *camPos = &this->bfData->viewport->camera->pos;

	QHashIterator<TankData*, bool> i(*this->bfData->activeTanks);
	while (i.hasNext()) {
		i.next();
		TankData *tankData = i.key();
		this->tankInterface->update(tankData, time, deltaMillis, deltaSec, t, camPos);
	}


	/*
	if (this->bfData->localUser != nullptr)
	{
		TankData *localTankData = this->tankInterface->getTankData(this->bfData->localUser);
		if (this->bfData->activeTanks->value(localTankData) == false)
		{
			this->tankInterface->update(localTankData, time, deltaMillis, deltaSec, t, &camPos);
		}
	}
	*/

}




void BattlefieldModel::runPhysics(uint32_t dt)
{
	//var key:* = undefined;
	//var tankData:* = null;
	if ((int(this->bfData->time) - int(this->bfData->physTime)) > MAX_FRAME_TIME)
	{

		this->bfData->physTime = this->bfData->time - MAX_FRAME_TIME;

	}

	if (this->bfData->physTime < this->bfData->time)
	{
		QHashIterator<TankData*, bool> i(*this->bfData->activeTanks);
		while (i.hasNext()) {
			i.next();
			TankData *tankData = i.key();
			tankData->tankCollisionCount = 0;
			tankData->tank->new_contact_normal = false;
			tankData->tank->normal_contact_new = false;
		}

		while (this->bfData->physTime < this->bfData->time)
		{
			//int time = getTimer();
			this->bfData->physicsScene->update(dt);
			this->bfData->physTime += dt;
			//this->updateTimeStat((getTimer() - time), this->physicsTimeStat, "Physics avrg time", "Physics avrg fps");
			if (((!(TankData::localTankData == nullptr)) && (!(TankData::localTankData->tank == nullptr))))
			{
				this->triggers->check(TankData::localTankData->tank);
			}
		}
	}
}


void BattlefieldModel::initMap(const QString &mapResourceId, const QString &skyboxId)
{
#ifdef GRAPHICS
	this->bfData->skybox = this->createSkyBox(skyboxId);
	this->bfData->viewport->setSkyBox(this->bfData->skybox.get());
#else
	(void)skyboxId;
#endif
	*this->mapResourceId = mapResourceId;
	this->config = new Config();
	this->config->load("mapsLibrary.json", mapResourceId);
}


void BattlefieldModel::addTankToField(TankData *tankData)
{
	//(Main.osgi.getService(IConsole) as IConsole).addLine("addTankToField() start");
	try
	{

		if (this->bfData->activeTanks->contains(tankData))
			return;

		this->bfData->activeTanks->insert(tankData, true);
		tankData->tank->addToContainer(this->bfData->viewport->getMapContainer());
		tankData->tank->updateSkin(1);
		tankData->tank->setAnimationTracks(this->animatedTracks);
		//tankData->logEvent("Added to field");

		this->bfData->physicsScene->addBody(tankData->tank);
		this->bfData->collisionDetector->addBody(tankData->tank);

		//this->_soundManager.addEffect(tankData.sounds);

		for (int i = 0; i < this->pluginCount; i++)
			this->plugins->at(i)->addUserToField(tankData->user);


		// && (!(this->activeCameraController == this->freeCameraController)
		if (tankData->tank == this->followCameraController->getTank())
		{
			this->followCameraController->activate();
			this->followCameraController->setLocked(false);
			this->followCameraController->initCameraComponents();
			this->activeCameraController = this->followCameraController;
		}
#ifdef GRAPHICS
		this->dusts->addTank(tankData);
#endif
		TankSkin *skin = tankData->tank->skin;

		this->objects2Tank->add(skin->turretMesh, tankData->tank);
		this->objects2Tank->add(skin->hullMesh, tankData->tank);
	}
	catch(int)
	{
		//(Main.osgi.getService(IConsole) as IConsole).addLine(e.getStackTrace());
	}
}

void BattlefieldModel::updatePanelUnlockCounter()
{
	/*
	if (this->panelUnlockCounter == 0)
	{
		return;
	}
	this->panelUnlockCounter--;
	if (this->panelUnlockCounter == 0)
	{
		IModelService *modelService = IModelService(Main.osgi.getService(IModelService));
		IPanel *panelModel = IPanel(modelService.getModelsByInterface(IPanel)[0]);
		panelModel.partSelected(4);
	}
	*/
}

void BattlefieldModel::addMainListeners()
{
	//if (this->bfData == null)
	//return;
	//this->bfData.guiContainer.addEventListener(Event.ENTER_FRAME, this->loop);
	//this->bfData.guiContainer.stage.addEventListener(Event.RESIZE, this->onResize);
	startLoop();
}

void BattlefieldModel::removeMainListeners()
{
	//if (this->bfData == null)
	//return;
	//this->bfData.guiContainer.removeEventListener(Event.ENTER_FRAME, this->loop);
	//this->bfData.guiContainer.stage.removeEventListener(Event.RESIZE, this->onResize);
	stopLoop();
}

void BattlefieldModel::startLoop()
{
	//PreciseTimer,
	//CoarseTimer,
	//VeryCoarseTimer

	if (loop_id != 0)
		return;

#ifdef GRAPHICS
	timer_loop->setTimerType(Qt::PreciseTimer);
	timer_loop->setInterval(16);
	loop_id = object_loop->startTimer(16, Qt::PreciseTimer);
#else
	timer_loop->setInterval(50);
	loop_id = object_loop->startTimer(50, Qt::PreciseTimer);
#endif

	//timer_loop->start();
}

void BattlefieldModel::stopLoop()
{
	if (loop_id != 0)
		object_loop->killTimer(loop_id);
	loop_id = 0;

	timer_loop->stop();
}

void BattlefieldModel::objectLoaded(ClientObject *clientObject)
{
	(void)clientObject;
	//this->_soundManager = SoundManager.createSoundManager(this->bfData.ambientSound);
	//this->messages = new CTFMessages(3, 18, 18);
	//Main.contentUILayer.addChild(this->messages);
	//Main.stage.addEventListener(Event.RESIZE, this->onStageResize);
	IBattleSettings *settings = this->getBattleSettings();
	//this->muteSound = settings.muteSound;
	//this->onStageResize(null);
	//if (((!(this->muteSound)) && (settings.bgSound)))
	//this->bfData.ambientChannel = this->_soundManager.playSound(this->bfData.ambientSound, 0, 1000000, new SoundTransform(0.5));


	//this->addKeyboardListeners();
	//this->bfData.time = getTimer();
	//this->onResize(null);
	//var tankEventDispatcher:ITankEventDispatcher = ITankEventDispatcher(Main.osgi.getService(ITankEventDispatcher));
	//if (tankEventDispatcher != null)
	//{
	//tankEventDispatcher.addTankEventListener(TankEvent.KILLED, this);
	//tankEventDispatcher.addTankEventListener(TankEvent.SPAWNED, this);
	//}
	Service::getNetworker()->send("battle;get_init_data_local_tank");

	//else
	//{
	//BattleController.localTankInited = true;
	//Network(Main.osgi.getService(INetworker)).send("battle;spectator_user_init");
	//this->activateSpectatorCamera();
	//}

	if (settings->fog())
		this->bfData->viewport->enableFog();
	else
		this->bfData->viewport->disableSoftParticles();

	//if (settings->shadows())
	//this->bfData->viewport->enableShadows();
	//else
	//this->bfData->viewport->disnableShadows();

	//Мягкие частицы
	if (settings->useSoftParticle())
		this->bfData->viewport->enableSoftParticles();
	else
		this->bfData->viewport->disableSoftParticles();

	this->bfData->viewport->enableAmbientShadows(settings->shadowUnderTanks());

#ifdef GRAPHICS
	this->dusts->enabled = settings->dust();
	this->_gameMode->applyChangesBeforeSettings(settings);
#endif
	this->bfData->viewport->camera->useShadowMap = settings->shadows();

	this->addMainListeners();
}


void BattlefieldModel::build(std::shared_ptr<KDContainer> mapTree, std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> c, QVector<std::shared_ptr<Light3D>> *lights, QVector<ToyLightAnimation*> *lighting)
{
	(void)lights;
	(void)lighting;
	//this->lightings = lighting;
	if (mapTree != nullptr)
		this->bfData->viewport->setMapContainer(mapTree);
	//this->bfData.viewport.initLights(lights);
	this->bfData->collisionDetector->buildKdTree(c, nullptr);
	this->onMapBuildingComplete();
}


void BattlefieldModel::onMapBuildingComplete()
{
	//this->gui = (Main.osgi.getService(IBattlefieldGUI) as IBattlefieldGUI);
	this->doRender = true;
	this->incCameraUnlockCounter();
	//this->updatePanelUnlockCounter();
#ifdef GRAPHICS
	if (this->_gameMode != nullptr)
		this->_gameMode->applyChanges(this->bfData->viewport);
#endif
	this->objectLoaded(nullptr);
}

void BattlefieldModel::incCameraUnlockCounter()
{
	//this->cameraUnlockCounter = (this->cameraUnlockCounter + 1);
	//if (this->cameraUnlockCounter == 2)
	//this->doRender = true;
}

bool BattlefieldModel::considerBodies(Body *body1, Body *body2)
{
	if (body1->postCollisionPredicate != nullptr && body2->postCollisionPredicate == nullptr)
	{
		//1 NEWCOME
		//2 ACTIVE

		Tank *tank_1 = (Tank*)body1;
		Tank *tank_2 = (Tank*)body2;
		tank_1->tankData->tankCollisionCount++;

		tank_1->new_contact_normal = true;
		tank_2->normal_contact_new = true;
	}
	else if (body1->postCollisionPredicate == nullptr && body2->postCollisionPredicate != nullptr)
	{
		//1 ACTIVE
		//2 NEWCOME

		Tank *tank = (Tank*)body2;
		tank->tankData->tankCollisionCount++;

		/*
		Tank *tank_1 = (Tank*)body1;
		Tank *tank_2 = (Tank*)body2;

		tank_1->normal_contact_new = true;
		tank_2->new_contact_normal = true;
*/
	}

	/*
	if (((!(body1->postCollisionPredicate == nullptr)) && (body2->postCollisionPredicate == nullptr)))
	{
		TankData *tankData = this->bfData->tanks->value((Tank*)body1);
		tankData->tankCollisionCount++;
	}
	else
	{
		if (((body1->postCollisionPredicate == nullptr) && (!(body2->postCollisionPredicate == nullptr))))
		{
			TankData *tankData = this->bfData->tanks->value((Tank*)body2);
			tankData->tankCollisionCount++;
		}
	}
	*/

	return false;
}

int BattlefieldModel::getRespawnInvulnerabilityPeriod()
{
	return this->getBattlefieldData()->respawnInvulnerabilityPeriod;
}

void BattlefieldModel::printDebugValue(const QString &, const QString &)
{

}

ISoundManager *BattlefieldModel::soundManager()
{
	return nullptr;
}

void BattlefieldModel::tankHit(TankData *, Vector3 *, double)
{

}

void BattlefieldModel::addFollowCameraModifier(ICameraStateModifier *)
{

}

void BattlefieldModel::removeDecal(Decal *decal)
{
	this->bfData->viewport->removeDecal(decal);
}

ObjectPoolService *BattlefieldModel::getObjectPoolService()
{
	return objectPoolService;
}

ITankEventDispatcher* BattlefieldModel::getTankEventDispatcher()
{
	return tankEventDispatcher;
}



void BattlefieldModel::initFollowCamera(Vector3 *pivotPosition, Vector3 *targetDirection)
{
	this->followCameraController->activate();
	this->followCameraController->setLocked(true);
	this->followCameraController->initByTarget(pivotPosition, targetDirection);
	this->activeCameraController = this->followCameraController;
	this->incCameraUnlockCounter();
}

void BattlefieldModel::initCameraController(ICameraController *controller)
{
	this->followCameraController->deactivate();
	this->activeCameraController = controller;
	this->tankInterface->lockControls(true);
}

void BattlefieldModel::initFlyCamera(Vector3 *pivotPosition, Vector3 *targetDirection)
{
	(void)pivotPosition;
	(void)targetDirection;

	//if (this->activeCameraController == this->freeCameraController)
	//return;

	this->followCameraController->deactivate();
	Vector3 cameraPosition, cameraAngles;
	this->followCameraController->getCameraState(pivotPosition, targetDirection, &cameraPosition, &cameraAngles);
	this->flyCameraController->init(&cameraPosition, &cameraAngles, CAMERA_FLY_TIME);
	this->activeCameraController = this->flyCameraController;

}


void BattlefieldModel::spawnTank(TankData *tankData)
{
	//Logger.debug((("spawnTank(" + this->bfData.activeTanks[tankData]) + ")"));
	if (this->bfData->activeTanks->contains(tankData))
		return;

	this->addTankToField(tankData);
	//this->_soundManager.addEffect(tankData.sounds);
}


void BattlefieldModel::destroy()
{
	delete this;
}

void BattlefieldModel::addGraphicEffect(IGraphicEffect *effect)
{
	if (effect == nullptr || this->bfData == nullptr)
		return;

	this->bfData->graphicEffects->insert(effect, true);
	effect->addToContainer(this->bfData->viewport->getMapContainer());

	//alternativa.engine3d.core:Object3DContainer - Object3DContainer.addToList index(24)
	//alternativa.engine3d.core:Object3DContainer - Object3DContainer.addChild index(2)
	//alternativa.tanks.models.battlefield.scene3dcontainer:Object3DContainerProxy - Object3DContainerProxy.addChild index(2)
	//alternativa.tanks.models.sfx:AnimatedLightEffect - AnimatedLightEffect.addToContainer index(4)
	//alternativa.tanks.models.battlefield:BattlefieldModel - BattlefieldModel.addGraphicEffect index(34)
	//alternativa.tanks.models.sfx.shoot.railgun:RailgunSFXModel - RailgunSFXModel.createChargeLightEffect index(6)
}

void BattlefieldModel::addSound3DEffect(ISound3DEffect *)
{

}



void BattlefieldModel::killTank(TankData *tankData)
{
	//this->_soundManager.removeEffect(tankData.sounds);
	//this->_soundManager.killEffectsByOwner(tankData.user);

	QHashIterator<ISpecialEffect*, bool> i(*this->bfData->graphicEffects);
	while (i.hasNext()) {
		i.next();
		ISpecialEffect *effect = i.key();
		if (effect->getOwner() == tankData->user)
		{
			effect->kill();
		}
	}




	/*
	if (this->bfData.killSound != null)
	{
		sound3D = Sound3D.create(this->bfData.killSound, SoundOptions.nearRadius, SoundOptions.farRadius, SoundOptions.farDelimiter, this->tankExplosionVolume.value);
		turretMesh = tankData.tank.skin.turretMesh;
		this->addSound3DEffect(Sound3DEffect.create(this->objectPoolService.objectPool, null, new Vector3(turretMesh.x, turretMesh.y, turretMesh.z), sound3D, 0, this->tankExplosionStartPosition.value));
	}
	*/

	if (this->followCameraController->getTank() == tankData->tank)
		this->followCameraController->setLocked(true);
}

void BattlefieldModel::playSpecialEffects(int dt)
{
	GameCamera *camera = this->bfData->viewport->camera;
	QMutableHashIterator<ISpecialEffect*, bool> i(*this->bfData->graphicEffects);
	while (i.hasNext()) {
		i.next();
		ISpecialEffect *effect = i.key();
		if (!effect->play(dt, camera)) {
			effect->destroy();
			i.remove();
		}
	}

	//if (!this->muteSound)
	//this->_soundManager.updateSoundEffects(dt, camera);
}


void BattlefieldModel::removeTank(TankData *tankData)
{
	if (this->bfData == nullptr)
		return;

	this->removeTankFromField(tankData);
	this->bfData->tanks->remove(tankData->tank);

	if (this->followCameraController->getTank() == tankData->tank)
	{
		this->followCameraController->deactivate();
		//if (this->spectatorMode)
		//this->freeCameraController->playerCamera.unfocus();
	}

	{
		QMutableHashIterator<ISpecialEffect*, bool> i(*this->bfData->graphicEffects);
		while (i.hasNext()) {
			i.next();
			ISpecialEffect *effect = i.key();
			if (effect->getOwner() == tankData->user) {
				effect->destroy();
				i.remove();
			}
		}
	}

	for (int i = 0; i < this->pluginCount; i++)
		this->plugins->at(i)->removeUser(tankData->user);
}


void BattlefieldModel::battleRestart()
{
	QHashIterator<TankData*, bool> i(*this->bfData->activeTanks);
	while (i.hasNext()) {
		i.next();
		TankData *tankData = i.key();
		tankData->tank->removeFromContainer();
		this->bfData->physicsScene->removeBody(tankData->tank);
		this->bfData->collisionDetector->removeBody(tankData->tank);
		this->tankInterface->stop(tankData);
	}

	this->bfData->activeTanks->clear();

	this->bfData->time = GetGameTime();

	for (int i = 0; i < this->pluginCount; i++)
		this->plugins->at(i)->restartBattle();
}



void BattlefieldModel::battleFinish()
{
	//var bonus:IBonus;
	this->tankInterface->disableUserControls(true);
	QHashIterator<TankData*, bool> i(*this->bfData->activeTanks);
	while (i.hasNext()) {
		i.next();
		TankData *tankData = i.key();
		//tankData->tank->title->hideIndicators();
		this->tankInterface->stop(tankData);
		tankData->enabled = false;
	}


	//this->suicideIndicator.visible = false;
	this->tankInterface->resetIdleTimer(true);

	{
		QHashIterator<QString, IBonus*> i(*this->bfData->bonuses);
		while (i.hasNext()) {
			i.next();
			IBonus *bonus = i.value();
			bonus->destroy();
		}
		this->bfData->bonuses->clear();
	}




	for (int i = 0; i < this->pluginCount; i++)
		this->plugins->at(i)->finishBattle();
}


void BattlefieldModel::showSuicideIndicator(int time)
{
	(void)time;
	Service::getIMain()->showSuicideIndicator();
}

void BattlefieldModel::hideSuicideIndicator()
{
	Service::getIMain()->hideSuicideIndicator();
}



void BattlefieldModel::addPlugin(IBattlefieldPlugin *plugin)
{
	if (!this->plugins->contains(plugin)) {
		this->pluginCount++;
		this->plugins->append(plugin);
	}
}

void BattlefieldModel::removePlugin(IBattlefieldPlugin *plugin)
{
	if (this->plugins->removeOne(plugin))
		this->pluginCount--;
}


void BattlefieldModel::addBonus(ClientObjectBonus *clientObject, const QString &bonusId, const QString &bonusObjectId, const Vector3 *position, int disappearingTime)
{
	this->createBonusAndAttach(nullptr, bonusId, bonusObjectId, position, 0, true, clientObject, disappearingTime);
}

void BattlefieldModel::onBonusDropped(IBonus *bonus)
{
	Vector3 position;
	bonus->readBonusPosition(&position);
}

void BattlefieldModel::onTankCollision(IBonus *bonus)
{
	if (bonus->isFalling())
		this->onBonusDropped(bonus);
	tankInterface->mandatoryUpdate();
	this->attemptToTakeBonus(this->bfData->bfObject, bonus->getBonusId());
}

void BattlefieldModel::attemptToTakeBonus(ClientObject *bfObject, const QString &bonusId)
{
	(void)bfObject;
	(void)bonusId;
	TankData *localTankData = this->tankInterface->getTankData(this->bfData->localUser);
	const Vector3 &pos = localTankData->tank->state.pos;

	QString str;
	str.append("battle;attempt_to_take_bonus;{\"real_tank_position\":{\"y\":");
	str.append(double_to_string(pos.y));
	str.append(",\"z\":");
	str.append(double_to_string(pos.z));
	str.append(",\"x\":");
	str.append(double_to_string(pos.x));
	str.append("},\"bonus_id\":\"");
	str.append(bonusId);
	str.append("\"}");

	Service::getNetworker()->send(str);
	//battle;attempt_to_take_bonus;{"real_tank_position":{"y":3767.453367519331,"z":93.78708505732982,"x":-5872.7438037820475},"bonus_id":"nitro_6080"}
}

void BattlefieldModel::addTrigger(Trigger *trigger)
{
	this->triggers->add(trigger);
}

void BattlefieldModel::removeTrigger(Trigger *trigger)
{
	this->triggers->remove(trigger);
}


IBonus* BattlefieldModel::createBonusAndAttach(ObjectRegister *objectRegister,
											   const QString &bonusId,
											   const QString &bonusObjectId,
											   const Vector3 *position,
											   int livingTime,
											   bool isFalling,
											   ClientObjectBonus *clientObject,
											   int disappearingTime)
{
	(void)objectRegister;
	(void)bonusObjectId;

	ClientObjectBonus *bonusObject = clientObject;
	if (bonusObject == nullptr)
		return nullptr;

	//BonusCommonModel *bonusModel = new BonusCommonModel();
	BonusCommonModel::initObject(clientObject, "bonus_box_" + bonusId.split("_")[0], "cords", disappearingTime, "parachute_inner", "parachute");

	IBonus *bonus = BonusCommonModel::getBonus(bonusObject, bonusId, livingTime, isFalling);

	if (this->bfData == nullptr || this->bfData->bonuses == nullptr)
	{
		bonus->destroy();
		return nullptr;
	}

	this->bfData->bonuses->insert(bonus->getBonusId(), bonus);
	bonus->attach(position, this->bfData->physicsScene, this->bfData->viewport->getMapContainer(), this);
	return bonus;
}


void BattlefieldModel::updateBonuses(int time, int dt, double t)
{
	QMutableHashIterator<QString, IBonus*> i(*this->bfData->bonuses);
	while (i.hasNext()) {
		i.next();
		IBonus *bonus = i.value();
		if (!bonus->update(time, dt, t)) {
			bonus->destroy();
			i.remove();
		}
	}
}


void BattlefieldModel::removeBonus(ClientObject *clientObject, const QString &bonusId)
{
	(void)clientObject;

	if (this->bfData == nullptr)
		return;

	IBonus *bonus = this->bfData->bonuses->value(bonusId);
	if (bonus != nullptr)
		bonus->setRemovedState();
}

void BattlefieldModel::bonusTaken(ClientObject *clientObject, const QString &bonusId)
{
	(void)clientObject;

	if (this->bfData == nullptr)
		return;

	IBonus *bonus = this->bfData->bonuses->value(bonusId);
	if (bonus != nullptr)
		bonus->setTakenState();
}





std::shared_ptr<SkyBox> BattlefieldModel::createSkyBox(const QString &skyboxId)
{
#ifdef GRAPHICS

	QString _skyboxId = skyboxId;

	if (_skyboxId.isEmpty())
		_skyboxId = "skybox_default";

#if (L_GT == 1|| L_PRO == 1)

	std::shared_ptr<BitmapData> skyBoxTexture;

	QString path = "skybox/";

	if (_skyboxId == "skybox_default") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "def.jpg");
	} else if (_skyboxId == "skybox_desert") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "des.jpg");
	} else if (_skyboxId == "skybox_iran") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "iran.jpg");
	} else if (_skyboxId == "skybox_halloween") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "halloween.jpg");
	} else if (_skyboxId == "skybox_winter") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "winter.jpg");
	} else if (_skyboxId == "skybox_plato") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "plato.jpg");
	} else if (_skyboxId == "skybox_new_year") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "new_year.jpg");
	} else if (_skyboxId == "skybox_winter_evening") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "winter_evening.jpg");
	} else if (_skyboxId == "skybox_evening") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "evening.jpg");
	} else if (_skyboxId == "skybox_space") {
		skyBoxTexture = ResourceUtil::getResourceImageLocal(path + "space.jpg");
	} else {
		skyBoxTexture = std::make_shared<StubBitmapData>(0xFF0000);
	}

	std::shared_ptr<TextureMaterial> material = new_TextureMaterial();
	material->init();
	material->setTexture(skyBoxTexture);

	int SKYBOX_SIZE = 200000;
	std::shared_ptr<SkyBox> skyBox = new_SkyBox();
	skyBox->init(SKYBOX_SIZE, material, material, material, material, material, material, 0);

	double correction = 0.015;
	Matrix m;

	int sides[4] = {SkyBox::RIGHT, SkyBox::BACK, SkyBox::LEFT, SkyBox::FRONT};
	for (int i = 0; i < 4; i++)
	{
		m.identity();
		m.scale(1. / (6. + correction), 1);
		m.translate(i / 6., 0);
		skyBox->transformUV(sides[i], &m);
	}

	m.identity();
	m.scale(-1. / (6. + correction), -1);
	m.translate(5. / 6., 1.);
	skyBox->transformUV(SkyBox::TOP, &m);
	m.identity();
	m.scale((-1. / (6. + correction)), -1);
	m.translate(1, 1);
	skyBox->transformUV(SkyBox::BOTTOM, &m);


#else
	std::shared_ptr<BitmapData> part1 = ResourceUtil::getResourceImage(_skyboxId + "_1")->bitmapData->data;
	std::shared_ptr<BitmapData> part2 = ResourceUtil::getResourceImage(_skyboxId + "_2")->bitmapData->data;
	std::shared_ptr<BitmapData> part3 = ResourceUtil::getResourceImage(_skyboxId + "_3")->bitmapData->data;
	std::shared_ptr<BitmapData> part4 = ResourceUtil::getResourceImage(_skyboxId + "_4")->bitmapData->data;
	std::shared_ptr<BitmapData> part5 = ResourceUtil::getResourceImage(_skyboxId + "_5")->bitmapData->data;
	std::shared_ptr<BitmapData> part6 = ResourceUtil::getResourceImage(_skyboxId + "_6")->bitmapData->data;

	std::shared_ptr<TextureMaterial> texture1 = new_TextureMaterial();
	std::shared_ptr<TextureMaterial> texture2 = new_TextureMaterial();
	std::shared_ptr<TextureMaterial> texture3 = new_TextureMaterial();
	std::shared_ptr<TextureMaterial> texture4 = new_TextureMaterial();
	std::shared_ptr<TextureMaterial> texture5 = new_TextureMaterial();
	std::shared_ptr<TextureMaterial> texture6 = new_TextureMaterial();

	texture1->init();
	texture2->init();
	texture3->init();
	texture4->init();
	texture5->init();
	texture6->init();

	texture1->setTexture(part1);
	texture2->setTexture(part2);
	texture3->setTexture(part3);
	texture4->setTexture(part4);
	texture5->setTexture(part5);
	texture6->setTexture(part6);

	int SKYBOX_SIZE = 200000;
	std::shared_ptr<SkyBox> skyBox = new_SkyBox();
	skyBox->init(SKYBOX_SIZE, texture1, texture2, texture3, texture4, texture5, texture6, 0);

#endif

	Vector3 v(10, 3, 0);
	this->skyboxRevolver = std::make_shared<Object3DRevolver>(skyBox.get(), &v, GameModes::isSpace(this->_gameMode) ? 0.04 : 0);
	return skyBox;

#else
	(void)skyboxId;
	return nullptr;
#endif
}



void BattlefieldModel::onResize()
{
	if (this->bfData == nullptr)
		return;
#ifdef GRAPHICS
	double scrSize = this->screenSize / this->screenSizeSteps;
	Stage *stage = this->bfData->viewport->getStage();
	double w = stage->stageWidth();
	double h = stage->stageHeight();
	int sw = w * scrSize;
	int sh = h * scrSize;
	this->bfData->viewport->resize(sw, sh);
	//this->bfData->viewport->x = (0.5 * (w - sw));
	//this->bfData->viewport->y = (0.5 * (h - sh));
#endif

	//IBackgroundService *bgService = (Main.osgi.getService(IBackgroundService) as IBackgroundService);
	//if (bgService != null)
	//		bgService.drawBg(new Rectangle((0.5 * (w - sw)), (0.5 * (h - sh)), sw, sh));

	//this->bfData->viewport->overlay->graphics->clear();
	//if (this->screenSize < this->screenSizeSteps)
	//	this->border.draw(this->bfData.viewport.overlay.graphics, sw, sh);

	//this->suicideIndicator.x = (sw >> 1);
	//this->suicideIndicator.y = (sh >> 1);
}

IBattleSettings* BattlefieldModel::getBattleSettings()
{
	return Service::getPanelModel();
}
