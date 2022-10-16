#ifndef IBATTLEFIELD_H
#define IBATTLEFIELD_H

class TankData;
class ClientObject;
class Tank;
class BattlefieldData;
class ClientObjectUser;
class IBattlefieldPlugin;
class IGraphicEffect;
class ISound3DEffect;
class Vector3;
class TextureMaterial;
enum class RotationState;
class ICameraController;
class ICameraStateModifier;
class QString;
class ISoundManager;
class ObjectPoolService;
class ITankEventDispatcher;
class HidableGraphicObjects;
class BattleLogicUnits;


class IBattleField
{
public:
	virtual void addDecal(const Vector3*, const Vector3*, double, TextureMaterial*, RotationState) = 0;
	virtual BattlefieldData* getBattlefieldData() = 0;
	virtual void addTank(TankData*) = 0;
	virtual void removeTank(TankData*) = 0;
	virtual void addGraphicEffect(IGraphicEffect*) = 0;
	virtual void addSound3DEffect(ISound3DEffect*) = 0;
	virtual void setLocalUser(ClientObjectUser*) = 0;
	virtual void initFlyCamera(Vector3*, Vector3*) = 0;
	virtual void initFollowCamera(Vector3*, Vector3*) = 0;
	virtual void initCameraController(ICameraController*) = 0;
	virtual void setCameraTarget(Tank*) = 0;
	virtual void removeTankFromField(TankData*) = 0;
	virtual void addPlugin(IBattlefieldPlugin*) = 0;
	virtual void removePlugin(IBattlefieldPlugin*) = 0;
	virtual void showSuicideIndicator(int) = 0;
	virtual void hideSuicideIndicator() = 0;
	virtual int getRespawnInvulnerabilityPeriod() = 0;
	virtual void printDebugValue(const QString&, const QString&) = 0;
	virtual ISoundManager* soundManager() = 0;
	virtual void tankHit(TankData*, Vector3*, double) = 0;
	virtual void addFollowCameraModifier(ICameraStateModifier*) = 0;
	virtual ObjectPoolService* getObjectPoolService() = 0;
	virtual ITankEventDispatcher* getTankEventDispatcher() = 0;
	virtual HidableGraphicObjects* getHidableGraphicObjects() = 0;
	virtual BattleLogicUnits* getBattleLogicUnits() = 0;
};

#endif // IBATTLEFIELD_H
