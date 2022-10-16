#ifndef CLIENTOBJECTWEAPONFLAMETHROWER_H
#define CLIENTOBJECTWEAPONFLAMETHROWER_H

#include "clientobjectweapon.h"

class ClientObjectWeaponFlamethrower : public ClientObjectWeapon
{
	std::shared_ptr<FlamethrowerData> flamethrowerData;
	std::shared_ptr<FlamethrowerSFXData> flamethrowerSFXData;
public:

	ClientObjectWeaponFlamethrower(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);


	static FlamethrowerData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponFlamethrower*)clientObject)->flamethrowerData.get();
	}

	static FlamethrowerSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponFlamethrower*)clientObject)->flamethrowerSFXData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<FlamethrowerData> data)
	{
		((ClientObjectWeaponFlamethrower*)clientObject)->flamethrowerData = data;
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<FlamethrowerSFXData> data)
	{
		((ClientObjectWeaponFlamethrower*)clientObject)->flamethrowerSFXData = data;
	}
};

#endif // CLIENTOBJECTWEAPONFLAMETHROWER_H
