#ifndef INETWORKER_H
#define INETWORKER_H

class QString;
class INetworkListener;

class INetworker
{
public:
	void connect(const QString&, int);
	void addEventListener(INetworkListener*);
	void onConnected();
	void onDataSocket();
};

#endif // INETWORKER_H
