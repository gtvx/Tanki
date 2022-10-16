#ifndef LOBBY_H
#define LOBBY_H

#include "networking/INetworkListener.h"

class Network;
class QByteArray;
class QString;
class BattleSelectModel;
class GarageModelActivator;
class QJsonObject;


class Lobby : public INetworkListener
{
	Network *network;
	BattleSelectModel *battleSelect;
	bool listInited;
public:

	static bool firstInit;

	Lobby(Network *network);
	~Lobby();

	BattleSelectModel *getBattleSelect();
	GarageModelActivator *garage;

	bool onData(Command *command);

	void parseGarageItems(const QByteArray &json, const QString &src);

private:
	void parseAndInitPanelInfo(const QByteArray &json);
	void initPanel(int crystall, const QString &email, bool tester, const QString &name, int nextScore, int place, int rank, int rating, int score);
	void init();
	void parseAndInitBattlesList(const QByteArray &json);
	void parseBattleInfo(const QByteArray &json);
	void parseBattleInfo_2(const QByteArray &json);
	void parseBattle(const QByteArray &json);

	void initGarageItems(const QByteArray &json, const QString &src);
	void parseBattle_2(QJsonObject &parser);
	void parseAndInitBattlesList_2(const QByteArray &json);
};

#endif // LOBBY_H
