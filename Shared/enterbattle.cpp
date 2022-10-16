#include "enterbattle.h"


EnterBattle::EnterBattle()
{
	this->team_enable = false;
	this->team = BattleTeamType::NONE;
}

void EnterBattle::setName(const QString &name)
{
	this->name = name;
}

void EnterBattle::setTeam(BattleTeamType team)
{
	this->team = team;
	this->team_enable = true;
}
