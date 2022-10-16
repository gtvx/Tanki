#ifndef SHOWBATTLEINFO_H
#define SHOWBATTLEINFO_H

#include <QVector>
#include "../../battleservice/model/battletype.h"
#include "../../battleservice/model/team/UserInfoClient.h"


class ShowBattleInfo
{
public:
	QVector<UserInfoClient> users;

	QString name;
	QString battleId;
	QString previewId;

	int minRank;
	int maxRank;
	int timeLimit;
	int timeCurrent;
	int killsLimit;
	int scoreRed;
	int scoreBlue;
	int maxPeople;

	BattleType type;

	bool fullCash;
	bool paidBattle;
	bool withoutBonuses;
	bool userAlreadyPaid;
	bool autobalance;
	bool frielndyFie;
};


#endif // SHOWBATTLEINFO_H
