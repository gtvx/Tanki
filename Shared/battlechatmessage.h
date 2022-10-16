#ifndef BATTLECHATMESSAGE_H
#define BATTLECHATMESSAGE_H

#include <QString>
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"

class BattleChatMessage
{
public:
	enum class TYPE
	{
		SPECTATOR,
		SYSTEM,
		USER,
	};

	TYPE type;

	QString text;
	QString nickname;
	BattleTeamType team_type;
	int rank;
	bool team;

	void setSpectator(const QString &text);
	void setSystem(const QString &text);
	void setUser(const QString &text, const QString &nickname, int rank, bool team, BattleTeamType team_type);
};


#endif // BATTLECHATMESSAGE_H
