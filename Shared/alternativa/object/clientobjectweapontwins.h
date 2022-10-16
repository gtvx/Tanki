#ifndef CLIENTOBJECTWEAPONTWINS_H
#define CLIENTOBJECTWEAPONTWINS_H

#include "clientobjectweapon.h"

class ClientObjectWeaponTwins : public ClientObjectWeapon
{
	std::shared_ptr<PlasmaGunData> plasmaData;
	std::shared_ptr<PlasmaSFXData> plasmaSFXData;
public:
	ClientObjectWeaponTwins(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);



	static PlasmaGunData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponTwins*)clientObject)->plasmaData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<PlasmaGunData> data)
	{
		((ClientObjectWeaponTwins*)clientObject)->plasmaData = data;
	}

	static PlasmaSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponTwins*)clientObject)->plasmaSFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<PlasmaSFXData> data)
	{
		((ClientObjectWeaponTwins*)clientObject)->plasmaSFXData = data;
	}
};


#endif // CLIENTOBJECTWEAPONTWINS_H
