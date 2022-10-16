#ifndef CLIENTOBJECTWEAPONTHUNDER_H
#define CLIENTOBJECTWEAPONTHUNDER_H

#include "clientobjectweapon.h"

class ClientObjectWeaponThunder : public ClientObjectWeapon
{
	std::shared_ptr<ThunderData> thunderData;
	std::shared_ptr<ThunderSFXData> thunderSFXData;
public:
	ClientObjectWeaponThunder(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);



	static ThunderData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponThunder*)clientObject)->thunderData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<ThunderData> data)
	{
		((ClientObjectWeaponThunder*)clientObject)->thunderData = data;
	}

	static ThunderSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponThunder*)clientObject)->thunderSFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<ThunderSFXData> data)
	{
		((ClientObjectWeaponThunder*)clientObject)->thunderSFXData = data;
	}
};


#endif // CLIENTOBJECTWEAPONTHUNDER_H
