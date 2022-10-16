#include "clientobjectweaponterminator.h"


ClientObjectWeaponTerminator::ClientObjectWeaponTerminator(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler) :
	ClientObjectWeapon(id, clientClass, name, handler)
{

}
