#ifndef CLIENTOBJECTWEAPONSHAFT_H
#define CLIENTOBJECTWEAPONSHAFT_H

#include "clientobjectweapon.h"

class ClientObjectWeaponShaft : public ClientObjectWeapon
{
	std::shared_ptr<ShaftData> shaftData;
	std::shared_ptr<ShaftSFXData> shaftSFXModel;

public:
	ClientObjectWeaponShaft(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);


	static ShaftData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponShaft*)clientObject)->shaftData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<ShaftData> data)
	{
		((ClientObjectWeaponShaft*)clientObject)->shaftData = data;
	}

	static ShaftSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponShaft*)clientObject)->shaftSFXModel.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<ShaftSFXData> data)
	{
		((ClientObjectWeaponShaft*)clientObject)->shaftSFXModel = data;
	}
};

#endif // CLIENTOBJECTWEAPONSHAFT_H
