#ifndef CLIENTOBJECTWEAPONISIDA_H
#define CLIENTOBJECTWEAPONISIDA_H

#include "clientobjectweapon.h"

class ClientObjectWeaponIsida : public ClientObjectWeapon
{

	std::shared_ptr<HealingGunData> healingGunData;
	std::shared_ptr<HealingGunSFXData> healingGunSFXData;

public:
	ClientObjectWeaponIsida(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);


	static HealingGunData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponIsida*)clientObject)->healingGunData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<HealingGunData> data)
	{
		((ClientObjectWeaponIsida*)clientObject)->healingGunData = data;
	}

	static HealingGunSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponIsida*)clientObject)->healingGunSFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<HealingGunSFXData> data)
	{
		((ClientObjectWeaponIsida*)clientObject)->healingGunSFXData = data;
	}
};


#endif // CLIENTOBJECTWEAPONISIDA_H
