#include "clientobjectweapon.h"
#include <QStringList>
#include "alternativa/tanks/models/weapon/common/weaponcommondata.h"
#include "alternativa/tanks/models/weapon/weakening/WeaponWeakeningData.h"


ClientObjectWeapon::ClientObjectWeapon(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler) :
	ClientObject(id, clientClass, name, handler)
{
	weapon_name = new QString(id.split(QChar('_'))[0]);
}

ClientObjectWeapon::~ClientObjectWeapon()
{
	delete weapon_name;
}

