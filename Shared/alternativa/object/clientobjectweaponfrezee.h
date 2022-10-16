#ifndef CLIENTOBJECTWEAPONFREZEE_H
#define CLIENTOBJECTWEAPONFREZEE_H

#include "clientobjectweapon.h"

class ClientObjectWeaponFrezee : public ClientObjectWeapon
{
	std::shared_ptr<FreezeData> freezeData;
	std::shared_ptr<FreezeSFXData> freezeSFXData;
public:
	ClientObjectWeaponFrezee(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);


	static FreezeData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponFrezee*)clientObject)->freezeData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<FreezeData> data)
	{
		((ClientObjectWeaponFrezee*)clientObject)->freezeData = data;
	}

	static FreezeSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponFrezee*)clientObject)->freezeSFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<FreezeSFXData> data)
	{
		((ClientObjectWeaponFrezee*)clientObject)->freezeSFXData = data;
	}
};


#endif // CLIENTOBJECTWEAPONFREZEE_H
