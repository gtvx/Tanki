#ifndef FLAMETHROWERSFXMODEL_H
#define FLAMETHROWERSFXMODEL_H

#include "IFlamethrowerSFXModel.h"

class QString;
class ClientObjectWeapon;
class EffectsPair;
class TankData;
class Vector3;
class Object3D;
class IWeaponWeakeningModel;
class IBattleField;
class IMaterialRegistry;
class FlamethrowerModel;


class FlamethrowerSFXModel : public IFlamethrowerSFXModel
{
	IBattleField *battlefield;
	IMaterialRegistry *materialRegistry;
	FlamethrowerModel *flamethrowerModel;
public:
	FlamethrowerSFXModel();
	virtual ~FlamethrowerSFXModel();

	void initObject(ClientObjectWeapon *clientObject, const QString &fireTextureId, const QString &flameSoundId);

	void getSpecialEffects(TankData *shooterData, const Vector3 *localMuzzlePosition, Object3D *turret, IWeaponWeakeningModel *weakeningModel, EffectsPair *effectsPair) override;

	void objectLoaded(ClientObjectWeapon *clientObject);

	void objectUnloaded(ClientObjectWeapon *clientObject);
};

#endif // FLAMETHROWERSFXMODEL_H
