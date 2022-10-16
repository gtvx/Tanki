#include "battlelogicunits.h"
#include "LogicUnit.h"

BattleLogicUnits::BattleLogicUnits()
{

}

void BattleLogicUnits::addLogicUnit(LogicUnit *unit)
{
	this->units.insert(unit);
}

void BattleLogicUnits::removeLogicUnit(LogicUnit *unit)
{
	this->units.remove(unit);
}

void BattleLogicUnits::update(int deltaMsec, int deltaSec)
{
	//for (LogicUnit *logicUnit : this->units)
		//logicUnit->runLogic(deltaMsec, deltaSec);

	QSetIterator<LogicUnit*> i(this->units);
	while (i.hasNext())
	{
		LogicUnit *logicUnit = i.next();
		logicUnit->runLogic(deltaMsec, deltaSec);
	}

}
