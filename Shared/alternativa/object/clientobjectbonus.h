#ifndef CLIENTOBJECTBONUS_H
#define CLIENTOBJECTBONUS_H

#include "clientobject.h"
class BonusCommonData;

class ClientObjectBonus : public ClientObject
{
public:
	ClientObjectBonus(const QString &id, const QString &name);
	BonusCommonData *bonusData;
};

#endif // CLIENTOBJECTBONUS_H
