#ifndef DEATHREASON_H
#define DEATHREASON_H

class QString;

enum class DeathReason
{
	SUICIDE,
	KILLED_IN_BATTLE,
};

DeathReason DeathReason_fromString(const QString &str);

#endif // DEATHREASON_H
