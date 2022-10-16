#ifndef PLASMASFXMODEL_H
#define PLASMASFXMODEL_H


#include "../ICommonShootSFX.h"

class ClientObject;
class QString;
class EffectsPair;
class Vector3;
class Object3D;
class Camera3D;
class PlasmaSFXData;
class ClientObjectWeapon;
class IMaterialRegistry;



//extends PlasmaShootSFXModelBase implements IPlasmaShootSFXModelBase, ICommonShootSFX, IPlasmaSFX, IObjectLoadListener
class PlasmaSFXModel : public ICommonShootSFX
{
	IMaterialRegistry *materialRegistry;

public:
	PlasmaSFXModel();
	virtual ~PlasmaSFXModel();

	void initObject(ClientObjectWeapon *clientObject,
					const QString &explosionSoundId,
					const QString &explosionTextureId,
					const QString &plasmaTextureId,
					const QString &shotSoundId,
					const QString &shotTextureId);

	void createShotEffects(ClientObjectWeapon *clientObject,
						   const Vector3 *muzzleLocalPos,
						   Object3D *turret,
						   Camera3D *camera,
						   EffectsPair* effectsPair);

	void createExplosionEffects(ClientObjectWeapon *clientObject, const Vector3 *pos, Camera3D *camera, double weakeningCoeff, EffectsPair *effectsPair);
	PlasmaSFXData* getPlasmaSFXData(ClientObjectWeapon *clientObject);
	void objectLoaded(ClientObjectWeapon *clientObject);
	void objectUnloaded(ClientObject *clientObject);
};

#endif // PLASMASFXMODEL_H
