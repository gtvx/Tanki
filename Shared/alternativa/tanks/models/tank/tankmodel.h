#ifndef TANKMODEL_H
#define TANKMODEL_H

#include "_global.h"
#include "ITank.h"
#include "networking/INetworkListener.h"
#include <qglobal.h>
#include <memory>

enum class BattleTeamType;
class BattlefieldModel;
class TankParts;
class Tank;
class ClientTank;
class TankSpecification;
class Vector3D;
enum class DeathReason;
class QString;
class TankSoundScheme;
class TankResources;
class ClientObjectUser;
class ClientObjectHull;
class Bot;
class DefaultUserTitlesRender;
class IEffectsVisualizationModel;
class DamageEffect;
class ITankEventDispatcher;
class RotateTurretCommand;


//TankModelBase implements ITankModelBase, IObjectLoadListener, INetworkListener, IFocusListener, IPanelListener, ITank, IUserStatListener, IDumper, IChatListener

class TankModel : public ITank
{
	bool _userControlsEnabled;
	bool firstSpawn;
	bool correctionEnabled;
	bool readyToSpawnCommandSent;
	bool idleTimerEnabled;
	bool reverseBackTurn;

	int idleTime;
	int idleKickPeriod;
	int uiControlLockCount;

	BattlefieldModel *battlefield;
	DefaultUserTitlesRender *userTitlesRender;
	IEffectsVisualizationModel *effectsModel;
	DamageEffect *damageEffect;
	ITankEventDispatcher *tankDispatcher;

	int suicideTime;
	bool awaitingKillCommand;
	int activationTime;
	int nextCorrectionTime;
	bool paused;
	bool controlsLocked;
	int numTanks;
	double possz;
	double orientations;
	double linVels;
	double angVels;

public:

	static const int FORWARD;
	static const int BACK;
	static const int LEFT;
	static const int RIGHT;

	static const int TURRET_LEFT;
	static const int TURRET_RIGHT;
	static const int CENTER_TURRET;
	static const int REVERSE_TURN_BIT;

	QHash<QString, TankData*> *tankDataById;

	TankData *localUserData;
	uint32_t controlBits;

	TankModel();
	virtual ~TankModel();
	void setBattlefield(BattlefieldModel*);

	virtual TankData* getTankData(ClientObjectUser *clientObject);
	virtual void update(TankData*, int, int, double, double, const Vector3*) override;
	virtual void disableUserControls(bool) override;
	virtual void enableUserControls() override;
	virtual void stop(TankData*) override;
	virtual void resetIdleTimer(bool) override;
	virtual bool getUserControlsEnabled() override;
	virtual void readLocalTankPosition(Vector3*) override;
	virtual void effectStarted(ClientObjectUser*, int, int) override;
	virtual void effectStopped(ClientObjectUser*, int) override;
	bool isSameTeam(BattleTeamType teamType1, BattleTeamType teamType2);
	void initTank(std::shared_ptr<ClientObjectUser> &clientObject, ClientTank *clientTank, TankParts *parts, bool notificationOfEnter = true);
	void activateTank(ClientObjectUser *clientObject);
	void setNormalTextures(TankData *tankData);
	void activateTankCommand(ClientObjectUser *user);
	void getPhysicsState(Vector3 *pos, Vector3 *orient, Vector3 *linVel, Vector3 *angVel);
	void setControlState(TankData *tankData, int ctrlBits, bool forced = false);
	bool initObject(std::shared_ptr<ClientObjectUser>  &clientObject, const QString &battleId, double mass, double power, TankSoundScheme *soundScheme, TankParts *tankParts, TankResources *tankResources, double impactForce, double kickback, double turretRotationAcceleration, double turretRotationSpeed, const QString &nickname, int rank, const QString &turretId);
	std::shared_ptr<ClientObjectHull> initClientObject(const QString &id, const QString &name);
	void spawn(ClientObjectUser *clientObject, TankSpecification *spec, BattleTeamType teamType, Vector3 *pos, Vector3 *orientation, int health, int incarnationId);
	void prepareToSpawn(ClientObjectUser *clientObject, Vector3 *pos, Vector3 *orientation);
	void applyControlState(TankData *tankData, int ctrlBits, bool sendToServer = true);
	void onKey(int key, bool down);
	void move(ClientObjectUser *clientObject, Vector3 *pos, Vector3 *orientation, Vector3 *linearVelocity, Vector3 *angleVelocity, double turretAngle, int control, bool isControl);
	void kill(ClientObjectUser *clientObject, DeathReason reason, const QString &killerTankId);
	void objectUnloaded(ClientObjectUser *clientObject);	
	void changeSpecification(ClientObjectUser *clientObject, const TankSpecification &tankSpecification, bool immediate);
	void moveAnticheat(ClientObjectUser *clientObject, Vector3 *pos, Vector3 *orientation, Vector3 *linearVelocity, Vector3 *angleVelocity, double turretAngle, int control, bool isControl);
	void startSuicideCountdown();	
	void mandatoryUpdate();
	void setTemperature(ClientObjectUser *clientObject, double value);
	void changeHealth(ClientObjectUser *clientObject, int newHealth);
	void createCriticalHitEffect(const QString &userId);
	void createDamageEffect(Tank *tank, int damage, uint32_t color);
	void lockControls(bool lock);
	void createLevelUpEffect(const QString &userId, int rang);
	void movementControl(ClientObjectUser *user, int control);
	void rotateTurret(ClientObjectUser *user, RotateTurretCommand *rotateTurretCommand);

private:
	void updateLocalTank(int time, int timeDelta);
	void rotateTurret(TankData *tankData, double deltaSec);
	void initLocalTank(TankData *tankData);
	void setBackTurnMode();
	Tank *createTank(TankData *tankData, TankParts *cl);
	void setTankSpec(TankData *tankData, const TankSpecification *spec, bool immediate);
	void putTankIntoBattle(TankData *tankData, ClientTank *clientTank);
	void configureRemoteTankTitle(TankData *tankData);
	void sendSuicideCommand(Vector3 *pos);
	void moveCommand(ClientObjectUser *obj, int time, const Vector3 *pos, const Vector3 *orientation, const Vector3 *linVel, const Vector3 *angVel, double turretDir, int controlBits);
	void setNormalState(TankData *tankData);
	void updateUILock(int lockCountDelta);
	void setTankState(TankData *tankData, const Vector3 *pos, const Vector3 *orient, const Vector3 *linVel, const Vector3 *angVel, double turretDir, int control, bool isControl);
	void setTankHealth(TankData *tankData, int health);
	void setDeadTextures(TankData *tankData);
	void suicideCommand(ClientObject *user, DeathReason reason, Vector3 *pos);
	bool createData(std::shared_ptr<ClientObjectUser> &clientObject, const QString &battleId, double mass, double power, TankSoundScheme *soundScheme, TankParts *tankParts, TankResources *tankResources, double impactForce, double kickback, double turretRotationAcceleration, double turretRotationSpeed, const QString &nickname, int rank, const QString &turretId);
	void setTransparentState(TankData *tankData);
	void createDeathEffects(TankData *tankData);

	void doDropFlag();
#ifdef L_OT
	void moveCorrectCommand(ClientObjectUser *obj, int time, Vector3 *pos, Vector3 *orientation, Vector3 *linVel, Vector3 *angVel, double turretDir, int controlBits);
#endif
	void setTankTemperature(Tank *tank, double temperature);
	void configureOtherTankTitles();
	void initModel();
};

#endif // TANKMODEL_H
