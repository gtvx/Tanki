#include "IsisActionType.h"
#include <QString>


IsisActionType IsisActionTypeFromString(const QString &s)
{
	if (s == "idle")
		return IsisActionType::IDLE;

	if (s == "damage")
		return IsisActionType::DAMAGE;

	if (s == "heal")
		return IsisActionType::HEAL;

	return IsisActionType::IDLE;
}
