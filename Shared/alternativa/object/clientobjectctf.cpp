#include "clientobjectctf.h"


ClientObjectCTF::ClientObjectCTF(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler) :
	ClientObject(id, clientClass, name, handler)
{
	initParams = nullptr;
}
