#ifndef ISISACTIONTYPE_H
#define ISISACTIONTYPE_H

class QString;

enum class IsisActionType
{
	HEAL,
	DAMAGE,
	IDLE,
};

IsisActionType IsisActionTypeFromString(const QString &);

#endif // ISISACTIONTYPE_H
