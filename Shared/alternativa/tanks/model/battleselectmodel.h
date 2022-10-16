#ifndef BATTLESELECTMODEL_H
#define BATTLESELECTMODEL_H

class Network;
class QString;
class ClientObject;

class BattleSelectModel
{
	Network *network;
public:
	BattleSelectModel(Network *network);
	void selectBattle(const QString &id);
	void fightBattle(const QString &battleId, bool team, bool red);
	void getDataInitBattleSelect();
	void startLoadBattle();
	void objectLoaded(ClientObject *object);
	void objectUnloaded(ClientObject *object);
};

#endif // BATTLESELECTMODEL_H
