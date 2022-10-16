#ifndef CLIENTOBJECTWEAPONRICOCHET_H
#define CLIENTOBJECTWEAPONRICOCHET_H

#include "clientobjectweapon.h"

class ClientObjectWeaponRicochet : public ClientObjectWeapon
{
	std::shared_ptr<RicochetData> ricochetData;
	std::shared_ptr<RicochetSFXData> ricochetSFXData;
public:
	ClientObjectWeaponRicochet(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);

	static RicochetData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponRicochet*)clientObject)->ricochetData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<RicochetData> data)
	{
		((ClientObjectWeaponRicochet*)clientObject)->ricochetData = data;
	}

	static RicochetSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponRicochet*)clientObject)->ricochetSFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<RicochetSFXData> data)
	{
		((ClientObjectWeaponRicochet*)clientObject)->ricochetSFXData = data;
	}
};

#endif // CLIENTOBJECTWEAPONRICOCHET_H
