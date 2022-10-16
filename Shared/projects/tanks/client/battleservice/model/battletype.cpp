#include "battletype.h"
#include <QString>

BattleType BattleType_getType(const QString &str)
{
	if (str == "CTF")
		return BattleType::CTF;
	else if (str == "TDM")
		return BattleType::TDM;
	else if (str == "DOM")
		return BattleType::DOM;
	return BattleType::DM;
}

QString BattleType_toString(BattleType type)
{
	switch (type)
	{
		case BattleType::CTF:
			return "CTF";
		case BattleType::TDM:
			return "TDM";
		case BattleType::DM:
			return "DM";
		case BattleType::DOM:
			return "DOM";
		default:
			return "";
	}
}
