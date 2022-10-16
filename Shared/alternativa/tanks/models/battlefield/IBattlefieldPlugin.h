#ifndef IBATTLEFIELDPLUGIN_H
#define IBATTLEFIELDPLUGIN_H

class QString;
class ClientObjectUser;

class IBattlefieldPlugin
{
public:
	virtual QString getBattlefieldPluginName() = 0;
	virtual void startBattle() = 0;
	virtual void restartBattle() = 0;
	virtual void finishBattle() = 0;
	virtual void tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff) = 0;
	virtual void addUser(ClientObjectUser*) = 0;
	virtual void removeUser(ClientObjectUser*) = 0;
	virtual void addUserToField(ClientObjectUser*) = 0;
	virtual void removeUserFromField(ClientObjectUser*) = 0;
	virtual ~IBattlefieldPlugin() {}
};

#endif // IBATTLEFIELDPLUGIN_H
