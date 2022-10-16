#include "clientobjecthull.h"

ClientObjectHull::ClientObjectHull(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler) :
	ClientObject(id, clientClass, name, handler)
{
	tankExplosionModel = nullptr;
	tankCriticalHitModel = nullptr;
}
