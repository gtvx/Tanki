#ifndef NETWORK_H
#define NETWORK_H

#include "networkservice.h"

class NetworkPrivate;
class Settings;

class Network : public NetworkService
{
	NetworkPrivate *d;
	Settings const *settings;
public:
	Network(const Settings *settings);
	~Network();
	void connect();
	void close();
	void send(const QString &data);
	void setAesData(const QByteArray &data);
	const Settings* getSettings();
	void acceptPackage(const QByteArray &data);
	void sendSystemDependenciesLoaded(int id);
};

#endif // NETWORK_H
