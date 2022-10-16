#ifndef CTFMODEL_H
#define CTFMODEL_H

#include "alternativa/math/vector3.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "alternativa/tanks/models/battlefield/IBattlefieldPlugin.h"
#include <memory>


class ClientObject;
class QString;
class Vector3;
class CaptureTheFlagSoundFX;
class ClientObjectCTF;
class BattlefieldData;
class ClientFlag;
class FlagData;
class FlagsState;
class BattlefieldModel;
class TankModel;
class TankData;
class ObjectRegister;
class BattleController;
class Object3D;
class CTFFlag;
class IMaterialRegistry;


class CTFModel : public IBattlefieldPlugin
{
	Vector3 posRedFlag;
	Vector3 posBlueFlag;
	BattlefieldData *bfData;
	BattlefieldModel *battlefieldModel;
	TankModel *tankModel;
	BattleController *battleController;
	TankData *userTankData;
	IMaterialRegistry *materialRegistry;

	int flagLockTime;
	bool dropCommandSent;

public:

	CTFFlag *flag_red;
	CTFFlag *flag_blue;
	std::shared_ptr<Object3D> pedestal_red;
	std::shared_ptr<Object3D> pedestal_blue;

	CTFModel(BattleController *battleController);
	virtual ~CTFModel();
	void initObject(ClientObjectCTF *clientObject, const QString &blueFlagModelPedestalId, const QString &blueFlagTexturePedestalId, const QString &blueFlagTextureId, const QString &redFlagModelPedestalId, const QString &redFlagTexturePedestalId, const QString &redFlagTextureId, CaptureTheFlagSoundFX *sounds, Vector3 *posBlueFlag, Vector3 *posRedFlag);
	void initFlagsState(ClientObjectCTF *clientObject, FlagsState *state);
	void objectLoaded(ClientObjectCTF *clientObject);
	QString getBattlefieldPluginName() override;
	void startBattle() override;
	void restartBattle() override;
	void finishBattle() override;
	void tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff) override;
	void addUser(ClientObjectUser*) override;
	void removeUser(ClientObjectUser*) override;
	void addUserToField(ClientObjectUser*) override;
	void removeUserFromField(ClientObjectUser*) override;

	void flagTaken(ClientObject *clientObject, const QString &tankId, BattleTeamType flagTeam);

	void returnFlagToBase(ClientObject *clientObject, BattleTeamType flagTeam, const QString &tankId);
	void flagDelivered(ClientObject *clientObject, BattleTeamType winnerTeam, const QString &delivererTankId);
	void dropFlag(ClientObject *clientObject, Vector3 *position, BattleTeamType flagTeam);
	void doDropFlag();
	void objectUnloaded(ClientObject *);
private:
	CTFFlag* initFlag(BattleTeamType teamType, ClientFlag *flagSpec, FlagData *flagData);
	TankData *getTankData(const QString &userId);
	void testFlagIntersection(CTFFlag *flag);
	void attemptToTakeFlagCommand(ClientObject *bfObject, BattleTeamType flagTeamType);
	CTFFlag *getFlag(BattleTeamType flagTeam);
	void returnFlag(BattleTeamType flagTeam);
	void dropFlagCommand(ClientObject *bfObject, Vector3 *pos3);
};

#endif // CTFMODEL_H
