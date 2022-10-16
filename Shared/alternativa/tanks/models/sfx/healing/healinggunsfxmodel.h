#ifndef HEALINGGUNSFXMODEL_H
#define HEALINGGUNSFXMODEL_H

#include <QHash>

class IBattleField;
class IWeaponCommonModel;
class ClientObjectWeapon;
class QString;
class TankData;
enum class IsisActionType;
class HealingGunSFX;


class HealingGunSFXModel
{
	IBattleField *battlefield;
	IWeaponCommonModel *weaponCommonModel;
	//targetPosLocal:Vector3 = new Vector3();
	//activeEffects:Dictionary = new Dictionary();

	QHash<QString, QHash<QString, HealingGunSFX*>*> activeEffects;

public:
	HealingGunSFXModel();

	void initObject(ClientObjectWeapon *clientObject,
					const QString &damagingRayId,
					const QString &damagingSoundId,
					const QString &damagingTargetBallId,
					const QString &damagingWeaponBallId,
					const QString &healingRayId,
					const QString &healingSoundId,
					const QString &healingTargetBallId,
					const QString &healingWeaponBallId,
					const QString &idleSoundId,
					const QString &idleWeaponBallId);

	void createOrUpdateEffects(TankData *ownerData, IsisActionType mode, TankData *targetData);
	void destroyEffectsByOwner(TankData *ownerData);
	void destroyEffectsByTarget(TankData *targetData);
};

#endif // HEALINGGUNSFXMODEL_H
