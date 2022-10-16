#ifndef CLIENTOBJECTWEAPON_H
#define CLIENTOBJECTWEAPON_H

#include "clientobject.h"
#include <memory>

class WeaponCommonData;
class SmokyModel;
class FlamethrowerModel;
class WeaponWeakeningData;
class FlamethrowerData;
class GunShootSFXData;
class FlamethrowerSFXData;
class PlasmaGunData;
class PlasmaSFXData;
class ThunderData;
class ThunderSFXData;
class HealingGunData;
class HealingGunSFXData;
class ShaftData;
class ShaftSFXData;
class FreezeData;
class FreezeSFXData;
class RailgunData;
class RailgunShootSFXData;
class TerminatorData;
class RicochetData;
class RicochetSFXData;


class ClientObjectWeapon : public ClientObject
{
public:
	QString *weapon_name;

	std::shared_ptr<WeaponCommonData> weaponCommonData;
	std::shared_ptr<WeaponWeakeningData> weaponWeakeningData;

	ClientObjectWeapon(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);
	virtual ~ClientObjectWeapon();
};

#endif // CLIENTOBJECTWEAPON_H
