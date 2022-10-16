#include "battlemode.h"
#include <QString>

BattleMode BattleModeFromString(const QString &mode)
{
	if (mode == "DM")
		return BattleMode::DM;

	if (mode == "TDM")
		return BattleMode::TDM;

	if (mode == "CTF")
		return BattleMode::CTF;

	if (mode == "CP")
		return BattleMode::CP;

	return BattleMode::DM;
}
