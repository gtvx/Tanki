#include "BattleTeamType.h"
#include <QString>

BattleTeamType BattleTeamType_getType(const QString &str)
{
	if (str == "RED")
		return BattleTeamType::RED;

	if (str == "BLUE")
		return BattleTeamType::BLUE;

	return BattleTeamType::NONE;
}

bool BattleTeamType_fromType(const QString &str, BattleTeamType *type)
{
	if (str == "RED") {
		*type = BattleTeamType::RED;
		return true;
	}

	if (str == "BLUE") {
		*type = BattleTeamType::BLUE;
		return true;
	}

	if (str == "NONE" || str == "GREEN") {
		*type = BattleTeamType::NONE;
		return true;
	}

	return false;
}

QString BattleTeamType_toString(BattleTeamType type)
{
	if (type == BattleTeamType::RED)
		return "RED";

	if (type == BattleTeamType::BLUE)
		return "BLUE";

	return "NONE";
}
