#include "networkservice.h"
#include "commands/command.h"
#include "INetworkListener.h"
#include "_global.h"
#include <QString>
#include <QStringList>
#include <QVectorIterator>
#include <QDebug>


NetworkService::NetworkService()
{
	listeners = new QVector<INetworkListener*>;
}

NetworkService::~NetworkService()
{
	delete listeners;
}

void NetworkService::sendRequestToAllListeners(Command *command)
{
	_function_name("NetworkService::sendRequestToAllListeners");

	try
	{
		for (INetworkListener *item : *listeners)
			if (item->onData(command) == true)
				break;
	}
	catch(int e)
	{
		qDebug() << "error" << e << command->readAll().mid(0, 100);
	}
}

void NetworkService::addListener(INetworkListener *listener)
{
	if (!listeners->contains(listener))
		listeners->append(listener);
}

void NetworkService::removeListener(INetworkListener *listener)
{
	listeners->removeOne(listener);
}
