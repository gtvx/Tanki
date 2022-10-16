#ifndef CLIENTOBJECTWEAPONSMOKY_H
#define CLIENTOBJECTWEAPONSMOKY_H

#include "clientobjectweapon.h"

class ClientObjectWeaponSmoky : public ClientObjectWeapon
{
	std::shared_ptr<GunShootSFXData> smokySFXData;
public:

	ClientObjectWeaponSmoky(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);

	static GunShootSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponSmoky*)clientObject)->smokySFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<GunShootSFXData> data)
	{
		((ClientObjectWeaponSmoky*)clientObject)->smokySFXData = data;
	}
};

#endif // CLIENTOBJECTWEAPONSMOKY_H
