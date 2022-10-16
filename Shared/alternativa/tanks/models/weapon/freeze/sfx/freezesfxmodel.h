#ifndef FREEZESFXMODEL_H
#define FREEZESFXMODEL_H

#include "IFreezeSFXModel.h"
#include <QHash>


class QString;
class ClientObjectWeapon;
class TankData;
class WeaponCommonData;
class Vector3;
class ClientObject;
class Object3D;
class OmniStreamLightEffect;
class IBattleField;
class FreezeModel;
class BattlefieldModel;
class FreezeGraphicEffect;


class FreezeSFXModel : public IFreezeSFXModel
{
	FreezeModel *freezeModel;
	//graphicEffects:Dictionary = new Dictionary();
	//soundEffects:Dictionary = new Dictionary();
	BattlefieldModel *bfModel;
	OmniStreamLightEffect *muzzleLightEffect;
	OmniStreamLightEffect *lightEffect;

	QHash<TankData*, FreezeGraphicEffect*> graphicEffects;

public:
	FreezeSFXModel();
	virtual ~FreezeSFXModel();


	void initObject(ClientObjectWeapon *clientObject,
					double particleSpeed,
					const QString &particleTextureResourceId,
					const QString &planeTextureResourceId,
					const QString &shotSoundResourceId);

	void createEffects(TankData *tankData, WeaponCommonData *weaponCommonData) override;
	void createLightEffect(const Vector3 *pos, Object3D *turret, ClientObject *turretObject);
	void destroyEffects(TankData *tankData) override;
	void objectLoaded(ClientObject *object);
	void objectUnloaded(ClientObject *object);
};

#endif // FREEZESFXMODEL_H
