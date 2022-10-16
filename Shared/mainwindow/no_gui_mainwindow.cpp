#include "no_gui_mainwindow.h"
#include "tank_bot/botsettings.h"
#include "FrameLobby/no_gui_framelobby.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "tank_bot/bot.h"
#include "battlechatmessage.h"
#include "settings.h"
#include <QDebug>



NoGuiMainWindow::NoGuiMainWindow(const Settings *settings)
{
	rank = 0;
	bot = nullptr;
	botSettings = new BotSettings;
	botSettings->shoot = settings->bot_shoot;
	botSettings->carry_flag = settings->bot_carry_flag;
	botSettings->capture_points = settings->bot_capture_points;
	botSettings->auto_kill = settings->bot_auto_kill;
	botSettings->random_path = settings->bot_random_path;
	botSettings->target_tank = settings->bot_target_tank;
	botSettings->body_turn = settings->bot_body_turn;
	botSettings->shoot = settings->bot_shoot;
	botEnable = settings->bot;

	frameLobby = nullptr;
	_enterBattle = nullptr;
	showLobby();
}

NoGuiMainWindow::~NoGuiMainWindow()
{
	delete botSettings;
}

void NoGuiMainWindow::showLobby()
{
	hideBattle();
	if (frameLobby != nullptr)
		return;
	frameLobby = new NoGuiFrameLobby;
	frameLobby->setRank(rank);
	frameLobby->battleEntrance(_enterBattle);
}

void NoGuiMainWindow::hideLobby()
{
	if (frameLobby == nullptr)
		return;
	delete frameLobby;
	frameLobby = nullptr;
}

void NoGuiMainWindow::showBattle()
{

}

void NoGuiMainWindow::hideBattle()
{

}

void NoGuiMainWindow::setNick(const QString &nick)
{
	(void)nick;
}

void NoGuiMainWindow::setRank(int rank)
{
	this->rank = rank;
	if (frameLobby != nullptr)
		frameLobby->setRank(rank);
}

void NoGuiMainWindow::setCrystal(int crystal)
{
	(void)crystal;
}

void NoGuiMainWindow::clearBattles()
{
	frameLobby->clearBattles();
}

void NoGuiMainWindow::addBattle(const BattleClient &battle)
{
	frameLobby->addBattle(battle);
}

void NoGuiMainWindow::showBattleInfo(const ShowBattleInfo &b)
{
	frameLobby->showBattleInfo(b);
}

void NoGuiMainWindow::addPlayerToBattle(const QString &battleId, const UserInfoClient &user)
{
	frameLobby->addPlayerToBattle(battleId, user);
}

void NoGuiMainWindow::removePlayerFromBattle(const QString &battleId, const QString &id)
{
	frameLobby->removePlayerFromBattle(battleId, id);
}

void NoGuiMainWindow::updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople)
{
	frameLobby->updateCountUsersInTeamBattle(battleId, redPeople, bluePeople);
}

void NoGuiMainWindow::updateCountUsersInDmBattle(const QString &battleId, int people)
{
	frameLobby->updateCountUsersInDmBattle(battleId, people);
}

void NoGuiMainWindow::removeBattle(const QString &battleId)
{
	frameLobby->removeBattle(battleId);
}

void NoGuiMainWindow::getSize(int *width, int *height)
{
	*width = 500.;
	*height = 500.;
}

void NoGuiMainWindow::initTank(Tank *tank)
{
	if (tank->tankData->local == true) {
		tank->botSettings = botSettings;
		this->bot = tank->initBot();
		this->bot->loadPoints();
		this->bot->setEnable(this->getBotEnable());
	}
}

void NoGuiMainWindow::deleteTank(Tank *tank)
{
	if (bot != nullptr) {
		bot->deleteTank(tank);
	}

	if (tank->tankData->local == true) {
		bot = nullptr;
	}
}

void NoGuiMainWindow::createDamageEffect(Tank *tank, int damage, uint32_t color)
{
	(void)tank;
	(void)damage;
	(void)color;
}

bool NoGuiMainWindow::getBotEnable()
{
	return botEnable;
}

void NoGuiMainWindow::showSuicideIndicator()
{

}

void NoGuiMainWindow::hideSuicideIndicator()
{

}

void NoGuiMainWindow::battleChatMessage(const BattleChatMessage &message)
{
	(void)message;
	qDebug() << message.nickname << message.text;
}

void NoGuiMainWindow::battleEntrance(EnterBattle *enterBattle)
{
	_enterBattle = enterBattle;
	if (frameLobby != nullptr)
		frameLobby->battleEntrance(enterBattle);
}

void NoGuiMainWindow::draw()
{

}

void NoGuiMainWindow::socketConnected()
{

}

void NoGuiMainWindow::socketDisconnected()
{

}

void NoGuiMainWindow::noConnection()
{

}

void NoGuiMainWindow::wrongPassword()
{

}

void NoGuiMainWindow::captcha()
{

}

void NoGuiMainWindow::ban()
{

}

void NoGuiMainWindow::lobbyShowGarage(GarageModel*)
{

}

void NoGuiMainWindow::log(const QString &)
{

}

bool NoGuiMainWindow::renderEnable()
{
	return false;
}

void NoGuiMainWindow::init()
{

}
