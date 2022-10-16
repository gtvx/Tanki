#ifndef ISISACTION_H
#define ISISACTION_H

#include <QString>
#include "../IsisActionType.h"

class IsisAction
{
public:
	QString shooterId;
	QString targetId;
	IsisActionType type;
};

#endif // ISISACTION_H
