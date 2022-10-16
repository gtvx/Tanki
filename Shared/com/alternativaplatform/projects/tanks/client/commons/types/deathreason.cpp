#include "deathreason.h"
#include <QString>

DeathReason DeathReason_fromString(const QString &str)
{
	if (str == "suicide")
		return DeathReason::SUICIDE;
	else
		return DeathReason::KILLED_IN_BATTLE;
}
