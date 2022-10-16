#ifndef BATTLEINFOCC_H
#define BATTLEINFOCC_H

#include "Range.h"
#include "battlemode.h"
#include "BattleLimits.h"

class BattleInfoCC
{
public:
	BattleMode battleMode;
	QString itemId;
	BattleLimits limits;
	//IGameObject map;
	int maxPeopleCount;
	QString name;
	bool proBattle;
	Range rankRange;
	bool roundStarted;
	bool spectator;
	int timeLeftInSec;
	bool userPaidNoSuppliesBattle;
	bool withoutBonuses;
	bool withoutCrystals;
	bool withoutSupplies;
	bool withoutUpgrades;
};


#endif // BATTLEINFOCC_H
