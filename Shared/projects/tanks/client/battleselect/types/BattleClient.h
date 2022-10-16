#ifndef BATTLECLIENT_H
#define BATTLECLIENT_H

#include <QString>

class BattleClient
{
public:
	QString battleId;
	QString mapId;
	QString name;

	int countRedPeople;
	int countBluePeople;
	int countPeople;
	int maxPeople;
	int minRank;
	int maxRank;

	bool team;
	bool paid;
};

#endif // BATTLECLIENT_H
