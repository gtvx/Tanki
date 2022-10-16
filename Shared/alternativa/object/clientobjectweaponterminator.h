#ifndef CLIENTOBJECTWEAPONTERMINATOR_H
#define CLIENTOBJECTWEAPONTERMINATOR_H

#include "clientobjectweapon.h"

class ClientObjectWeaponTerminator : public ClientObjectWeapon
{
	std::shared_ptr<TerminatorData> terminatorData;
	std::shared_ptr<RailgunShootSFXData> railgunSFXData;
public:
	ClientObjectWeaponTerminator(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);


	static TerminatorData* data(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponTerminator*)clientObject)->terminatorData.get();
	}

	static void setData(ClientObjectWeapon *clientObject, std::shared_ptr<TerminatorData> data)
	{
		((ClientObjectWeaponTerminator*)clientObject)->terminatorData = data;
	}

	static RailgunShootSFXData* SWFData(ClientObjectWeapon *clientObject)
	{
		return ((ClientObjectWeaponTerminator*)clientObject)->railgunSFXData.get();
	}

	static void setSWFData(ClientObjectWeapon *clientObject, std::shared_ptr<RailgunShootSFXData> data)
	{
		((ClientObjectWeaponTerminator*)clientObject)->railgunSFXData = data;
	}
};


#endif // CLIENTOBJECTWEAPONTERMINATOR_H
