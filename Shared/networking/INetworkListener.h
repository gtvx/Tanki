#ifndef INETWORKLISTENER_H
#define INETWORKLISTENER_H

class Command;

class INetworkListener
{
public:
	virtual bool onData(Command*) = 0;
	virtual ~INetworkListener() {}
};

#endif // INETWORKLISTENER_H
