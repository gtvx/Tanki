#ifndef BATTLEITEMCC_H
#define BATTLEITEMCC_H

#include <QString>
#include "Range.h"
#include "battlemode.h"

class BattleItemCC
{
public:
	QString battleId;
	BattleMode battleMode;
	//IGameObject map;
	int maxPeople;
	QString name;
	bool noSuppliesBattle;
	bool privateBattle;
	bool proBattle;
	Range rankRange;
	bool suspicious;
};


#endif // BATTLEITEMCC_H
