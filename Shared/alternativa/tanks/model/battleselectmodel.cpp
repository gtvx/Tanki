#include "battleselectmodel.h"
#include "networking/network.h"
#include "service.h"
#include "IMain.h"
#include "battlecontroller.h"
#include "networking/network.h"
#include "__global.h"
#include <QString>


BattleSelectModel::BattleSelectModel(Network *network)
{
	this->network = network;
}


void BattleSelectModel::selectBattle(const QString &id)
{
#ifndef L_PRO
	network->send("lobby;get_show_battle_info;" + id);
#else
	network->send("battle_select;select;" + id);
#endif
}


void BattleSelectModel::fightBattle(const QString &battleId, bool team, bool red)
{
#ifndef L_PRO
	QString str;
	str.append("lobby;");
	str.append(team ? "enter_battle_team;" : "enter_battle;");
	str.append(battleId);
	str.append(QChar(';'));
	str.append(red ? "true" : "false");
	network->send(str);
#else
	(void)battleId;
	(void)team;
	(void)red;
	network->send("battle_select;fight");
#endif
}

void BattleSelectModel::getDataInitBattleSelect()
{
	Service::getIMain()->showLobby();
	network->send("lobby;get_data_init_battle_select");
}

void BattleSelectModel::startLoadBattle()
{
	//this->objectUnloaded(null);
	//PanelModel(Main.osgi.getService(IPanel)).startBattle(null);
	//ChatModel(Main.osgi.getService(IChatModelBase)).objectUnloaded(null);

	BattleController *battleController = Service::getBattleController();
	if (battleController != nullptr)
		return;

	battleController = new BattleController(network);
	Service::setBattleController(battleController);
	network->addListener(battleController);
}


void BattleSelectModel::objectLoaded(ClientObject*)
{

}

void BattleSelectModel::objectUnloaded(ClientObject*)
{
	//(BattleSelectModelActivator.osgi.getService(IDumpService) as IDumpService).unregisterDumper(this->dumperName);
	/*
	if (this->addressService != nullptr)
	{
		this->addressService.removeEventListener(SWFAddressEvent.CHANGE, this->onAddressChange);
		if (this->clearURL)
		{
			this->addressService.setValue("");
		}
	}
	*/

	//this->hideList();
	//this->hideInfo();
	//this->hideCreateForm();
	//Main.stage.removeEventListener(Event.RESIZE, this->alignHelpers);
	//this->helpService.hideHelper(this->HELPER_GROUP_KEY, this->HELPER_NOT_AVAILABLE);
	//this->helpService.unregisterHelper(this->HELPER_GROUP_KEY, this->HELPER_NOT_AVAILABLE);
	//this->helpService.unregisterHelper(this->HELPER_GROUP_KEY, this->HELPER_CREATE_MAP);
	//this->lockedMapsHelper = null;
	//this->createHelper = null;
	//this->battleList = null;
	//this->createBattleForm = null;
	//this->viewDM = null;
	//this->viewTDM = null;
	//this->battlesArray = null;
	//this->mapsArray = null;
	//this->battles = null;
	//this->maps = null;
	//this->usersInfo = null;
	//this->paidBattleAlert = null;
}
