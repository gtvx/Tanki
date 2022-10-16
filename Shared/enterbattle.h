#ifndef ENTERBATTLE_H
#define ENTERBATTLE_H

#include <QString>
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"

class EnterBattle
{
	QString name;
	BattleTeamType team;
	bool team_enable;
public:
	EnterBattle();
	void setName(const QString &name);
	QString& getName() { return name; }

	void setTeam(BattleTeamType team);
	bool isTeam() { return team_enable; }
	BattleTeamType getTeam() { return team; }
};

#endif // ENTERBATTLE_H
