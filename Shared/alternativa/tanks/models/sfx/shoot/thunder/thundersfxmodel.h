#ifndef THUNDERSFXMODEL_H
#define THUNDERSFXMODEL_H

#include "IThunderSFXModel.h"

class QString;
class Vector3;
class Object3D;
class EffectsPair;


class ThunderSFXModel : public IThunderSFXModel
{
public:
	ThunderSFXModel();
	virtual ~ThunderSFXModel();

	static void initObject(ClientObjectWeapon *clientObject,
						   const QString &explosionResourceId,
						   const QString &explosionSoundResourceId,
						   const QString &shotResourceId,
						   const QString &shotSoundResourceId);

	void createShotEffects(ClientObjectWeapon *clientObject, const Vector3 *muzzleLocalPos, Object3D *turret, EffectsPair *effectsPair) override;
	void createExplosionEffects(ClientObjectWeapon *clientObject, const Vector3 *position, EffectsPair *effectsPair) override;
};

#endif // THUNDERSFXMODEL_H
