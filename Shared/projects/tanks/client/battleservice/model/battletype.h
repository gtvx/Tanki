#ifndef BATTLETYPE_H
#define BATTLETYPE_H

class QString;

enum class BattleType
{
	CTF,
	TDM,
	DM,
	DOM,
};

BattleType BattleType_getType(const QString &str);
QString BattleType_toString(BattleType type);

#endif // BATTLETYPE_H
