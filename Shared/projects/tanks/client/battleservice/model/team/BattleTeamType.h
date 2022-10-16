#ifndef BATTLETEAMTYPE_H
#define BATTLETEAMTYPE_H

class QString;

enum class BattleTeamType
{
	NONE,
	RED,
	BLUE,
};

BattleTeamType BattleTeamType_getType(const QString &str);
bool BattleTeamType_fromType(const QString &str, BattleTeamType *type);
QString BattleTeamType_toString(BattleTeamType type);

#endif // BATTLETEAMTYPE_H
