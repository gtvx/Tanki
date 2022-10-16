#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <qglobal.h>
#include "commands/CommandType.h"

class INetworkListener;
class Network;
class Command;


class NetworkService
{
	QVector<INetworkListener*> *listeners;
public:
	NetworkService();
	~NetworkService();
	void sendRequestToAllListeners(Command *command);
	void addListener(INetworkListener *listener);
	void removeListener(INetworkListener *listener);
};

#endif // NETWORKSERVICE_H
