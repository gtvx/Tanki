#ifndef THUNDERMODEL_H
#define THUNDERMODEL_H

#include <QVector>
#include "alternativa/math/vector3.h"
#include <memory>
#include "../IWeaponController.h"

class ClientObject;
class ClientObjectWeapon;
class QString;
class Vector3;
class TankData;
class ObjectRegister;
class HitInfo;
class Tank;
class ICollisionDetector;
class IBattleField;
class IWeaponCommonModel;
class TankModel;
class TextureMaterial;
class ShotData;
class WeaponCommonData;
class CommonTargetSystem;
class CommonTargetEvaluator;
class ThunderData;
class IThunderSFXModel;
class ClientObjectUser;


class ThunderModel : public IWeaponController
{
	std::shared_ptr<TextureMaterial> decalMaterial;
	//weaponUtils:WeaponUtils = WeaponUtils.getInstance();
	//modelService:IModelService;
	IBattleField *battlefieldModel;
	TankModel *tankInterface;
	IWeaponCommonModel *commonModel;
	//weakeningModel:IWeaponWeakeningModel;
	IThunderSFXModel *sfxModel;
	TankData *localTankData;
	ShotData *localShotData;
	ThunderData *localThunderData;
	WeaponCommonData *localWeaponCommonData;
	bool _triggerPressed;
	int nextReadyTime;
	CommonTargetSystem *targetSystem;
	//hitInfo:HitInfo = new HitInfo();
	//_hitPos:Vector3 = new Vector3();
	//_hitPosLocal:Vector3 = new Vector3();
	//_hitPosGlobal:Vector3 = new Vector3();
	Vector3 _gunDirGlobal;
	//_muzzlePosGlobal:Vector3 = new Vector3();
	Vector3 _barrelOrigin;
	//_xAxis:Vector3 = new Vector3();
	//_vector:Vector3 = new Vector3();
	//_rayOrigin:Vector3 = new Vector3();
	//_rayInteresction:RayIntersection = new RayIntersection();
	//_hitPos3d:Vector3d = new Vector3d(0, 0, 0);
	//_targetPos3d:Vector3d = new Vector3d(0, 0, 0);
	//_splashTargetIds:Array = [];
	//_splashTargetIncarnations:Array = [];
	//_splashTargetDistances:Array = [];
	//_splashTargetPositions:Array = [];
	//testPointsAux:Vector.<Vector3> = Vector.<Vector3>([new Vector3(), new Vector3(), new Vector3()]);
	//testPoints:Vector.<Vector3> = Vector.<Vector3>([new Vector3(), new Vector3(), new Vector3()]);
	CommonTargetEvaluator *targetEvaluator;
	double maxTargetingDistance = 100000;

	QVector<QString> _splashTargetIds;
	QVector<int> _splashTargetIncarnations;
	QVector<double> _splashTargetDistances;
	QVector<Vector3*> _splashTargetPositions;


public:
	ThunderModel();
	virtual ~ThunderModel();


	void initObject(ClientObjectWeapon *clientObject,
					double impactForce,
					double maxSplashDamageRadius,
					double minSplashDamagePercent,
					double minSplashDamageRadius);

	void fire(ClientObjectUser *clientObject,
			  const QString &shooterId,
			  const Vector3 *hitPoint,
			  const QString &mainTargetId,
			  double weakeningCoeff,
			  QVector<QString> &splashTargetIds);

	void stopEffects(TankData *ownerTankData) override;
	void reset() override;
	void setLocalUser(TankData *localUserData) override;
	void clearLocalUser() override;

public:
	void activateWeapon(int time) override;
	void deactivateWeapon(int time, bool sendServerCommand) override;
	double update(int time, int deltaTime) override;
	void objectLoaded(ClientObjectWeapon *object);
	void objectUnloaded(ClientObject *object);

private:
	double getSplashImpactCoeff(double distance);
	TankData* getTankData(const QString &objectId);
	void createShotEffects(TankData *tankData, const Vector3 *muzzlePosLocal);
	void createExplosionEffects(TankData *tankData, const Vector3 *position);
	bool testForSplashHit(Tank *tank, HitInfo *hitInfo, ICollisionDetector *collisionDetector);
	void applyMainImpactForce(Tank *target, const Vector3 *globalPos, const Vector3 *gunDir, double commonImpact, double thunderImpact, double weakeningCoeff);
};

#endif // THUNDERMODEL_H
