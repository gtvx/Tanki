#ifndef FRAMELOBBY_H
#define FRAMELOBBY_H

#include <QFrame>
#include "projects/tanks/client/battleservice/model/battletype.h"


class BattleClient;
class ShowBattleInfo;
class UserInfoClient;
class EnterBattle;

namespace Ui {
	class FrameLobby;
}

class FrameLobby : public QFrame
{
	Q_OBJECT

public:
	explicit FrameLobby(QWidget *parent = nullptr);
	~FrameLobby();

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

private slots:
	void on_treeWidget_battles_itemSelectionChanged();
	void on_pushButton_play_red_clicked();
	void on_pushButton_play_blue_clicked();

private:
	Ui::FrameLobby *ui;
	QString currentBattleId;
	QString currentBattleName;
	BattleType currentBattleType;
	EnterBattle *_enterBattle;
	int rank;
	int maxPeople;
	int currentPeopleRedGreen;
	int currentPeopleBlue;
	bool battle_enable;
	void autoEntrance(bool free_green_red, bool free_blue);
};

#endif // FRAMELOBBY_H
