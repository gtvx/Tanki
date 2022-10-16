#ifndef WEAPONSMANAGER_H
#define WEAPONSMANAGER_H

#include <QVariantMap>

class IWeapon;
class ClientObject;
class ClientObjectWeapon;
class QString;
class ShotData;
class ClientObjectUser;
class ICommonShootSFX;
class FlamethrowerSFXModel;
class PlasmaSFXModel;
class ThunderSFXModel;
class IThunderSFXModel;
class HealingGunSFXModel;
class ShaftSFXModel;
class FreezeSFXModel;
class RailgunSFXModel;
class RicochetSFXModel;


class WeaponsManager
{
public:
	static IWeapon *getWeapon(ClientObjectUser *owner, ClientObjectWeapon *clientObject, double impactForce, double kickback, double turretRotationAcceleration, double turretRotationSpeed);
	static void shotDataInsert(const QString &id, ShotData *shotdata);
	static ShotData* shotData(const QString &id);
	static ClientObjectWeapon *getObjectFor(const QString &id);
	static QVariantMap getSpecialEntity(const QString &id);
	static void setSpecialEntity(const QString &id, const QVariantMap &);
	static void clear();

	static ICommonShootSFX* getCommonShotSFX(ClientObjectWeapon *turret);
	static FlamethrowerSFXModel* getFlamethrowerSFX(ClientObjectWeapon *clientObject);
	static PlasmaSFXModel* getTwinsSFX(ClientObjectWeapon *clientObject);
	static IThunderSFXModel* getThunderSFX(ClientObjectWeapon *clientObject);
	static HealingGunSFXModel* getIsidaSFX(ClientObjectWeapon *clientObject);
	static ShaftSFXModel* getShaftSFX(ClientObjectWeapon *clientObject);
	static FreezeSFXModel* getFrezeeSFXModel(ClientObjectWeapon *clientObject);
	static RailgunSFXModel* getRailgunSFX(ClientObjectWeapon *clientObject);
	static RailgunSFXModel* getTerminatorSFX(ClientObjectWeapon *clientObject);
	static RicochetSFXModel* getRicochetSFXModel(ClientObjectWeapon *clientObject);
};

#endif // WEAPONSMANAGER_H
