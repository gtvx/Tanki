#ifndef CLIENTOBJECTHULL_H
#define CLIENTOBJECTHULL_H

#include "clientobject.h"
#include <memory>

class ExplosionData;

class ClientObjectHull : public ClientObject
{
public:

	std::shared_ptr<ExplosionData> tankExplosionModel;
	std::shared_ptr<ExplosionData> tankCriticalHitModel;

	ClientObjectHull(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler);
};

#endif // CLIENTOBJECTHULL_H
