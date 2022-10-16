#ifndef CLIENTOBJECTUSER_H
#define CLIENTOBJECTUSER_H

#include "clientobject.h"

class TankData;

class ClientObjectUser : public ClientObject
{
public:
	TankData *tankData;
	ClientObjectUser(const QString &id, const QString &name);
};

#endif // CLIENTOBJECTUSER_H
