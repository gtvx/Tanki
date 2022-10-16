#ifndef SMOKYSFXMODEL_H
#define SMOKYSFXMODEL_H

#include "alternativa/tanks/models/sfx/shoot/ICommonShootSFX.h"

class QString;

class SmokySFXModel : public ICommonShootSFX //public GunShootSFXModelBase, public IGunShootSFXModelBase,
{
public:

	SmokySFXModel();
	virtual ~SmokySFXModel();

	void initObject(ClientObjectWeapon *clientObject,
					const QString &explosionResourceId,
					const QString &explosionSoundResourceId,
					const QString &shotResourceId,
					const QString &shotSoundResourceId);

	void createShotEffects(ClientObjectWeapon *clientObject, const Vector3 *localMuzzlePosition, Object3D *turret, Camera3D *camera, EffectsPair *effectsPair) override;
	void createExplosionEffects(ClientObjectWeapon *clientObject, const Vector3 *position, Camera3D *camera, double weakeningCoeff, EffectsPair *effectsPair) override;
};

#endif // SMOKYSFXMODEL_H
