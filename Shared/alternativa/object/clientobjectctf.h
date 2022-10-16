#ifndef CLIENTOBJECTCTF_H
#define CLIENTOBJECTCTF_H

#include "clientobject.h"
class InitParams;

class ClientObjectCTF : public ClientObject
{
public:
	ClientObjectCTF(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);
	InitParams *initParams;
};

#endif // CLIENTOBJECTCTF_H
