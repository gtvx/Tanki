#ifndef CLIENTOBJECTWEAPONRAILGUN_H
#define CLIENTOBJECTWEAPONRAILGUN_H

#include "clientobjectweapon.h"

class ClientObjectWeaponRailgun : public ClientObjectWeapon
{
	std::shared_ptr<RailgunData> railgunData;
	std::shared_ptr<RailgunShootSFXData> railgunSFXData;
public:
	ClientObjectWeaponRailgun(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);

	static RailgunData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponRailgun*)clientObject)->railgunData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<RailgunData> data)
	{
		((ClientObjectWeaponRailgun*)clientObject)->railgunData = data;
	}

	static RailgunShootSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponRailgun*)clientObject)->railgunSFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<RailgunShootSFXData> data)
	{
		((ClientObjectWeaponRailgun*)clientObject)->railgunSFXData = data;
	}
};


#endif // CLIENTOBJECTWEAPONRAILGUN_H
