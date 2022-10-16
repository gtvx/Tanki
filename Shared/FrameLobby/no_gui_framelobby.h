#ifndef NO_GUI_FRAMELOBBY_H
#define NO_GUI_FRAMELOBBY_H

#include "projects/tanks/client/battleservice/model/battletype.h"
#include <QVector>
#include <QSharedPointer>


class BattleClient;
class ShowBattleInfo;
class UserInfoClient;
class BattleItem;
class BattleUserItem;
class EnterBattle;


class NoGuiFrameLobby
{
public:
	explicit NoGuiFrameLobby();
	~NoGuiFrameLobby();

	void clearBattles();
	void addBattle(const BattleClient &battle);
	void showBattleInfo(const ShowBattleInfo &b);
	void addPlayerToBattle(const QString &battleId, const UserInfoClient &user);
	void removePlayerFromBattle(const QString &battleId, const QString &id);
	void updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople);
	void updateCountUsersInDmBattle(const QString &battleId, int people);
	void removeBattle(const QString &battleId);
	void setRank(int rank);
	void battleEntrance(EnterBattle *enterBattle);



private:
	QString currentBattleId;
	QString currentBattleName;
	BattleType currentBattleType;
	EnterBattle *_enterBattle;
	int rank;
	int maxPeople;
	int currentPeopleRedGreen;
	int currentPeopleBlue;
	bool battle_enable;
	QVector<QSharedPointer<BattleItem>> battles;
	QVector<QSharedPointer<BattleUserItem>> reds;
	QVector<QSharedPointer<BattleUserItem>> blues;
	void on_pushButton_play_red_clicked();
	void on_pushButton_play_blue_clicked();
	void autoEntrance(bool free_green_red, bool free_blue);
};

#endif // NO_GUI_FRAMELOBBY_H
