#ifndef USERINFOCLIENT_H
#define USERINFOCLIENT_H

#include "BattleTeamType.h"
#include <QString>


class UserInfoClient
{
public:
	QString id;
	QString name;
	BattleTeamType type;
	int rank;
	int kills;
};

#endif // USERINFOCLIENT_H
