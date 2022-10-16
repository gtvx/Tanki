#ifndef BATTLEMODE_H
#define BATTLEMODE_H

class QString;

enum class BattleMode
{
	DM,
	TDM,
	CTF,
	CP,
};

BattleMode BattleModeFromString(const QString &mode);


#endif // BATTLEMODE_H
