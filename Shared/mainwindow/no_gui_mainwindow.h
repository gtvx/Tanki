#ifndef NO_GUI_MAINWINDOW_H
#define NO_GUI_MAINWINDOW_H

#include "IMain.h"
#include "projects/tanks/client/battleservice/model/battletype.h"
#include <QString>

class Vector3;
class Tank;
class QTreeWidgetItem;
class Damage;
class BotSettings;
class Bot;
class NoGuiFrameLobby;
class EnterBattle;
class Settings;


class NoGuiMainWindow : public IMain
{
public:
	NoGuiMainWindow(const Settings *settings);
	~NoGuiMainWindow();

	void showLobby() override;
	void hideLobby() override;
	void showBattle() override;
	void hideBattle() override;
	void setNick(const QString &nick) override;
	void setRank(int rank) override;
	void setCrystal(int crystal) override;
	void clearBattles() override;
	void addBattle(const BattleClient&) override;
	void showBattleInfo(const ShowBattleInfo&) override;
	void addPlayerToBattle(const QString &battleId, const UserInfoClient &) override;
	void removePlayerFromBattle(const QString &battleId, const QString &id) override;
	void updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople) override;
	void updateCountUsersInDmBattle(const QString &battleId, int people) override;
	void removeBattle(const QString &battleId) override;
	void getSize(int *width, int *height) override;
	void initTank(Tank*) override;
	void deleteTank(Tank *tank);
	void createDamageEffect(Tank *tank, int damage, uint32_t color) override;
	bool getBotEnable() override;
	void showSuicideIndicator() override;
	void hideSuicideIndicator() override;
	void battleChatMessage(const BattleChatMessage &message) override;
	void battleEntrance(EnterBattle *enterBattle);
	void draw() override;
	void socketConnected() override;
	void socketDisconnected() override;
	void noConnection() override;
	void wrongPassword() override;
	void captcha() override;
	void ban() override;
	void lobbyShowGarage(GarageModel *garageModel);
	void log(const QString&) override;
	bool renderEnable() override;



	void init();

private:
	int rank;
	Bot *bot;
	BotSettings *botSettings;
	NoGuiFrameLobby *frameLobby;
	EnterBattle *_enterBattle;
	bool botEnable;
};

#endif // NO_GUI_MAINWINDOW_H
