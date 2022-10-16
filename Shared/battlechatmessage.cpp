#include "battlechatmessage.h"

void BattleChatMessage::setSpectator(const QString &text)
{
	this->type = TYPE::SPECTATOR;
	this->text = text;
	this->rank = 0;
	this->team = false;
	this->team_type = BattleTeamType::NONE;
}

void BattleChatMessage::setSystem(const QString &text)
{
	this->type = TYPE::SYSTEM;
	this->text = text;
	this->rank = 0;
	this->team = false;
	this->team_type = BattleTeamType::NONE;
}

void BattleChatMessage::setUser(const QString &text, const QString &nickname, int rank, bool team, BattleTeamType team_type)
{
	this->type = TYPE::USER;
	this->text = text;
	this->nickname = nickname;
	this->rank = rank;
	this->team = team;
	this->team_type = team_type;
}
