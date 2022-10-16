#include "clientobjectbonus.h"
#include <QString>


ClientObjectBonus::ClientObjectBonus(const QString &id, const QString &name) :
	ClientObject(id, nullptr, name, nullptr)
{
	bonusData = nullptr;
}
