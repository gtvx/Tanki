#ifndef SHAFTSFXMODEL_H
#define SHAFTSFXMODEL_H

class ClientObjectWeapon;
class ClientObjectUser;
class QString;
class MobileSound3DEffect;
class ClientObject;
class Object3D;
class ShaftSFXData;
class Vector3;
class Class;
class BattlefieldModel;
class ObjectPoolService;


class ShaftSFXModel
{
	BattlefieldModel *battlefieldModel;
	ObjectPoolService *objectPoolService;
public:


	ShaftSFXModel();

	void initObject(ClientObjectWeapon *clientObject,
					const QString &manualModeEffect,
					const QString &targetingSound,
					const QString &explosionId,
					const QString &trailTexture,
					const QString &muzzleFlashTexture);

	MobileSound3DEffect* createManualModeEffects(ClientObjectWeapon *turretObj, ClientObjectUser *clientObject, Object3D *obj);
	void fadeChargingEffect(ClientObjectWeapon *turretObj, ClientObjectUser *clientObject);
	void createShotSoundEffect(ClientObjectWeapon *turretObj, ClientObjectUser *owner, Vector3 *param1);
	void createMuzzleFlashEffect(ClientObjectWeapon *turretObj, Vector3 *param1, Object3D *param2);
	void stopManualSound(ClientObjectUser *clientObject);
	void playTargetingSound(ClientObjectWeapon *turretObj, ClientObjectUser *clientObject, bool play);
	void createHitPointsGraphicEffects(ClientObjectWeapon *turretObject,
									   ClientObjectUser *owner,
									   const Vector3 *pos1,
									   const Vector3 *pos2,
									   Vector3 *param3,
									   Vector3 *param4,
									   Vector3 *param5);

private:
	void createEffectsForPoint(ClientObjectWeapon *turretObject,
							   ClientObjectUser *owner,
							   ShaftSFXData *sfxData,
							   const Vector3 *param1,
							   Vector3 *param2,
							   Vector3 *param3,
							   Vector3 *param4,
							   bool param5);

	void createExplosionGraphicEffect(ClientObjectWeapon *turretObject, ClientObjectUser *owner, ShaftSFXData *sfxData, const Vector3 *param1);
	void createExplosionSoundEffect(ClientObjectWeapon *turretObject, ClientObjectUser *owner, ShaftSFXData *sfxData, const Vector3 *param1);
	void createTrailEffect1(ShaftSFXData *sfxData, const Vector3 *position, const Vector3 *direction, double param4, double param5);
	void createTrailEffect2(ShaftSFXData *sfxData, const Vector3 *position, const Vector3 *direction, double param4, double param5);
};

#endif // SHAFTSFXMODEL_H
