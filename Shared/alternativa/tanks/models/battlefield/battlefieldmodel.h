#ifndef BATTLEFIELDMODEL_H
#define BATTLEFIELDMODEL_H

#include <qglobal.h>
#include "IBattleField.h"
#include "alternativa/physics/collision/IBodyCollisionPredicate.h"
#include "alternativa/tanks/bonuses/IBonusListener.h"
#include <memory>


class BattlefieldData;
class ICameraController;
class QTimer;
class QObject;
class TankModel;
class ClientObject;
class BattlefieldResources;
class BattlefieldSoundScheme;
class IGameMode;
class IFollowCameraController;
class FlyCameraController;
class ClientObjectUser;
class TanksCollisionDetector;
class KDContainer;
class CollisionPrimitive;
class Light3D;
class ToyLightAnimation;
class Vector3;
class Vector3D;
class IGraphicEffect;
class IBattlefieldPlugin;
class IBonus;
class ObjectRegister;
class ClientObjectBonus;
class Trigger;
class Triggers;
class BattleMinesModel;
class Object3DRevolver;
class SkyBox;
class Config;
class Decal;
class FadingDecalsRenderer;
class TankEventDispatcher;
class HidableGraphicObjects;
class BattleLogicUnits;
class Objects2Tank;
class IBattleSettings;
class Dust;


class BattlefieldModel : public IBattleField, public IBodyCollisionPredicate, public IBonusListener
{
	bool doRender;
	uint8_t fps_tfDelay;

	int screenSizeSteps;
	int screenSize;
	int loop_id;

	int32_t pluginCount;
	uint32_t physicsTime;
	uint32_t fps_tfTimer;

	ICameraController *activeCameraController;
	FlyCameraController *flyCameraController;
	QVector<IBattlefieldPlugin*> *plugins;
	Triggers *triggers;
	std::shared_ptr<Object3DRevolver> skyboxRevolver;
	ObjectPoolService *objectPoolService;
	TankEventDispatcher *tankEventDispatcher;
	HidableGraphicObjects *hidableObjects;
	BattleLogicUnits *logicUnits;
	Objects2Tank *objects2Tank;
	IGameMode *_gameMode;
	Dust *dusts;
	QObject *object;
	QObject *object_loop;
	QTimer *timer_loop;
	TankModel *tankInterface;
	QString *mapResourceId;
	Config *config;

	double deltaSec;
	double lastLogicUnitsUpdate;
	int64_t systemTimeStart;

public:
	BattlefieldModel();
	virtual ~BattlefieldModel();
	BattlefieldData *bfData;
	IFollowCameraController *followCameraController;
	BattleMinesModel *battleMinesModel;
	FadingDecalsRenderer *fadingDecalRenderer;
	bool animatedTracks;

	QString getMapResourceId();
	void initObject(ClientObject *clientObject, BattlefieldResources *battlefieldResources, BattlefieldSoundScheme *battlefieldSoundScheme, int idleKickPeriodMsec, const QString &mapDescriptorResourceId, int respawnInvulnerabilityPeriodMsec, const QString &skyboxTextureResourceId, bool spectator, IGameMode *gameMode);
	void build(std::shared_ptr<KDContainer> mapTree, std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> c, QVector<std::shared_ptr<Light3D>> *lights, QVector<ToyLightAnimation*> *lighting);

	void loop();

	void spawnTank(TankData *tankData);
	void destroy();
	void killTank(TankData *tankData);
	void battleRestart();
	void battleFinish();
	void addBonus(ClientObjectBonus *clientObject, const QString &bonusId, const QString &bonusObjectId, const Vector3 *position, int disappearingTime);

	void onBonusDropped(IBonus*);
	void onTankCollision(IBonus*);

	void addTrigger(Trigger *trigger);
	void removeTrigger(Trigger *trigger);

	void removeBonus(ClientObject *clientObject, const QString &bonusId);
	void bonusTaken(ClientObject *clientObject, const QString &bonusId);

	void objectUnloaded(ClientObject *);

	void resetFollowCamera();
	void activateFollowCamera();

	Config* getConfig() { return this->config; }

	HidableGraphicObjects* getHidableGraphicObjects() override;

	BattleLogicUnits* getBattleLogicUnits() override;

	void addDecal(const Vector3 *hitPosition, const Vector3 *muzzlePosition, double radius, TextureMaterial *material, RotationState rotationState) override;
	BattlefieldData* getBattlefieldData() override;
	void addTank(TankData*) override;
	void removeTank(TankData*) override;
	void addGraphicEffect(IGraphicEffect*) override;
	void addSound3DEffect(ISound3DEffect*) override;
	void setLocalUser(ClientObjectUser*) override;
	void initFlyCamera(Vector3 *pivotPosition, Vector3 *targetDirection) override;
	void initFollowCamera(Vector3 *pivotPosition, Vector3 *targetDirection) override;
	void initCameraController(ICameraController*) override;
	void setCameraTarget(Tank*) override;
	void removeTankFromField(TankData*) override;
	void addPlugin(IBattlefieldPlugin*) override;
	void removePlugin(IBattlefieldPlugin*) override;
	void showSuicideIndicator(int time) override;
	void hideSuicideIndicator() override;
	int getRespawnInvulnerabilityPeriod() override;
	void printDebugValue(const QString&, const QString&) override;
	ISoundManager* soundManager() override;
	void tankHit(TankData*, Vector3*, double) override;
	void addFollowCameraModifier(ICameraStateModifier*) override;
	void removeDecal(Decal *decal);
	ObjectPoolService* getObjectPoolService() override;
	ITankEventDispatcher* getTankEventDispatcher() override;
	IBattleSettings* getBattleSettings();
	IGameMode* gameMode() { return _gameMode; }

	Objects2Tank* getObjects2Tank() { return objects2Tank; }

	void onResize();


private:
	void updateTanks(uint32_t time, uint32_t deltaMillis, double deltaSec, double t);
	void runPhysics(uint32_t dt);
	void initMap(const QString &mapResourceId, const QString &skyboxId);
	void initPhysicsAndViewport();
	void addTankToField(TankData *tankData);
	void updatePanelUnlockCounter();
	void addMainListeners();
	void removeMainListeners();
	void objectLoaded(ClientObject *clientObject);
	void onMapBuildingComplete();
	void incCameraUnlockCounter();
	bool considerBodies(Body*, Body*);
	void levitation_control(Tank *tank, double timeDelta);
	void playSpecialEffects(int dt);
	IBonus *createBonusAndAttach(ObjectRegister *objectRegister, const QString &bonusId, const QString &bonusObjectId, const Vector3 *position, int intlivingTime, bool isFalling, ClientObjectBonus *clientObject, int disappearingTime);
	void attemptToTakeBonus(ClientObject *bfObject, const QString &bonusId);
	void updateBonuses(int time, int dt, double t);
	std::shared_ptr<SkyBox> createSkyBox(const QString &skyboxId);
	void startLoop();
	void stopLoop();
};

#endif // BATTLEFIELDMODEL_H
