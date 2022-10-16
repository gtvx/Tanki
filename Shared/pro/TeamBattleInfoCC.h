#ifndef TEAMBATTLEINFOCC_H
#define TEAMBATTLEINFOCC_H

#include <QVector>

class BattleInfoUser;

class TeamBattleInfoCC
{
public:

	bool autoBalance;
	bool friendlyFire;
	int scoreBlue;
	int scoreRed;
	QVector<BattleInfoUser*> usersBlue;
	QVector<BattleInfoUser*> usersRed;
};

#endif // TEAMBATTLEINFOCC_H
