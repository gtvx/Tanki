#ifndef BATTLELOGICUNITS_H
#define BATTLELOGICUNITS_H

#include <QSet>

class LogicUnit;

class BattleLogicUnits
{
	QSet<LogicUnit*> units;
public:
	BattleLogicUnits();

	void addLogicUnit(LogicUnit *unit);
	void removeLogicUnit(LogicUnit *unit);
	void update(int deltaMsec, int deltaSec);
};

#endif // BATTLELOGICUNITS_H
