#include "lobby.h"
#include "networking/commands/command.h"
#include "alternativa/tanks/model/panel/panelmodel.h"
#include "networking/network.h"
#include "projects/tanks/client/battleselect/types/mapclient.h"
#include "projects/tanks/client/battleselect/types/BattleClient.h"
#include "alternativa/tanks/model/battleselectmodel.h"
#include "projects/tanks/client/battleservice/model/team/UserInfoClient.h"
#include "projects/tanks/client/battleservice/model/battletype.h"
#include "projects/tanks/client/battleselect/types/showbattleinfo.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "service.h"
#include "IMain.h"
#include "battlecontroller.h"
#include "alternativa/init/garagemodelactivator.h"
#include "resource/resourceutil.h"
#include "com/alternativaplatform/projects/tanks/client/garage/item/itempropertyvalue.h"
#include "com/alternativaplatform/projects/tanks/client/garage/item/modificationinfo.h"
#include "alternativa/types/long.h"
#include "alternativa/tanks/model/garagemodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include "com/alternativaplatform/projects/tanks/client/commons/models/itemtype/itemtypeenumfromint.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/itempropertyfromstring.h"
#include "alternativa/tanks/model/itemparams.h"
#include "com/alternativaplatform/projects/tanks/client/garage/garage/iteminfo.h"
#include "__global.h"
#include "pro/BattleInfoCC.h"
#include "pro/ProBattleCC.h"
#include "pro/BattleInfoUser.h"
#include "pro/BattleDMInfoCC.h"
#include "pro/TeamBattleInfoCC.h"
#include "pro/BattleDMItemCC.h"
#include "pro/TeamBattleItemCC.h"
#include "pro/BattleItemCC.h"



bool Lobby::firstInit = true;


Lobby::Lobby(Network *network)
{
	this->network = network;
	this->listInited = false;
	this->battleSelect = new BattleSelectModel(network);

	this->garage = new GarageModelActivator();
	this->garage->start(nullptr);
}

Lobby::~Lobby()
{
	delete this->battleSelect;
	delete this->garage;
}


BattleSelectModel* Lobby::getBattleSelect()
{
	return battleSelect;
}


bool Lobby::onData(Command *command)
{
	CommandType type = command->type;

	if (type == CommandType::LOBBY_CHAT) {

		return true;
	} else if (type == CommandType::LOBBY) {

		//lobby;create_battle;{"isPaid":false,"battleId":"4905@242 TDM@#86","countPeople":0,"redPeople":0,"minRank":4,"name":"242 TDM","mapId":"map_242","team":true,"bluePeople":0,"maxPeople":8,"previewId":"map_242_preview","maxRank":8}


		QString cmd = command->readArgument();
		if (cmd == "init_panel") {
			this->parseAndInitPanelInfo(command->readRest());
		} else if (cmd == "init_battle_select") {

#ifndef L_PRO
			this->parseAndInitBattlesList(command->readRest());
#else
			this->parseAndInitBattlesList_2(command->readRest());
#endif

		} else if (cmd == "show_battle_info") {
#ifndef L_PRO
			this->parseBattleInfo(command->readRest());
#else
			this->parseBattleInfo_2(command->readRest());
#endif
		} else if (cmd == "remove_player_from_battle") {
			QJsonDocument doc = QJsonDocument::fromJson(command->readRest());
			QJsonObject obj = doc.object();
			QString battleId = obj.value("battleId").toString();
			QString id = obj.value("id").toString();
			Service::getIMain()->removePlayerFromBattle(battleId, id);
		} else if (cmd == "update_count_users_in_team_battle") {
			QJsonDocument doc = QJsonDocument::fromJson(command->readRest());
			QJsonObject obj = doc.object();
			QString battleId = obj.value("battleId").toString();
			int redPeople = obj.value("redPeople").toInt();
			int bluePeople = obj.value("bluePeople").toInt();
			Service::getIMain()->updateCountUsersInTeamBattle(battleId, redPeople, bluePeople);
		} else if (cmd == "update_count_users_in_dm_battle") {
			QString battleId = command->readArgument();
			int people = command->readArgument().toInt();
			Service::getIMain()->updateCountUsersInDmBattle(battleId, people);
		} else if (cmd == "add_player_to_battle") {
			QJsonDocument doc = QJsonDocument::fromJson(command->readRest());
			QJsonObject obj = doc.object();
			QString battleId = obj.value("battleId").toString();
			UserInfoClient info;
			info.id = obj.value("id").toString();
			info.kills = obj.value("kills").toInt();
			info.name = obj.value("name").toString();
			info.rank = obj.value("rank").toInt();
			info.type = BattleTeamType_getType(obj.value("type").toString());
			Service::getIMain()->addPlayerToBattle(battleId, info);
		} else if (cmd == "remove_battle") {
			Service::getIMain()->removeBattle(command->readRest());
		} else if (cmd == "start_battle") {

			this->battleSelect->startLoadBattle();

		} else if (cmd == "init_battlecontroller") {

			this->battleSelect->startLoadBattle();

		} else if (cmd == "create_battle") {

			this->parseBattle(command->readRest());

		} else if (cmd == "update_rang") {
			PanelModel *modelPanel = Service::getPanelModel();
			int rang = command->readArgument().toInt();
			int nextScore = command->readArgument().toInt();
			modelPanel->updateRank(nullptr, rang, nextScore);
		} else if (cmd == "add_crystall") {
			PanelModel *modelPanel = Service::getPanelModel();
			int crystal = command->readArgument().toInt();
			modelPanel->updateCrystal(nullptr, crystal);
		}

#ifdef L_PRO
		else if (cmd == "init_premium") {
			QByteArray json = command->readArgument();
			(void)json;
		}
#endif
		//recv lobby;change_layout_state;BATTLE
		//recv lobby;unload_battle_select
		//recv lobby;start_battle
		//recv lobby;unload_chat

		return true;
	} else if (type == CommandType::GARAGE) {
		QString cmd = command->readArgument();
		if (cmd == "init_garage_items") {
#ifdef GRAPHICS
			this->parseGarageItems(command->readRest(), "");
#else
			battleSelect->getDataInitBattleSelect();
#endif
		} else if (cmd == "init_mounted_item") {
			QString data = command->readRest();
			//ItemParams *item = GarageModel::getItemParams(data);
			this->garage->garageModel->mountItem(nullptr, nullptr, data);
		}


		return true;
	} else {
		return false;
	}
}


void Lobby::parseAndInitPanelInfo(const QByteArray &json)
{
	QJsonDocument doc = QJsonDocument::fromJson(json);
	QJsonObject obj = doc.object();
	int crystall = obj.value("crystall").toInt();
	QString email = obj.value("email").toString();
	bool tester = obj.value("tester").toBool();
	QString name = obj.value("name").toString();
	int next_score = obj.value("next_score").toInt();
	int place = obj.value("place").toInt();
	int rang = obj.value("rang").toInt();
	int rating = obj.value("rating").toInt();
	int score = obj.value("score").toInt();
	this->initPanel(crystall, email, tester, name, next_score, place, rang, rating, score);
}


void Lobby::initPanel(int crystall, const QString &email, bool tester, const QString &name, int nextScore, int place, int rank, int rating, int score)
{
	PanelModel *modelPanel = Service::getPanelModel();
	modelPanel->initObject(network, nullptr, crystall, email, tester, name, nextScore, place, rank, rating, score);
	//modelPanel->lock();
	//UserData *user1 = new UserData(name, name, rang);
	//Main.osgi.registerService(IUserData, user1);
	this->init();
}


void Lobby::init()
{
	//this->chat->start(Game.getInstance.osgi);
	//this->battleSelect->start(Game.getInstance.osgi);
}


void Lobby::parseAndInitBattlesList(const QByteArray &json)
{
	{
		BattleController *battleController = Service::getBattleController();
		if (battleController != nullptr) {
			battleController->destroy();
			Service::getIMain()->draw();
		}
	}


	//var js:Object;
	//var obj1:Object;
	//var btl:Object;
	//var map:MapClient;
	//var battle:BattleClient;
	//(Main.osgi.getService(IConsole) as IConsole).addLine("server parseAndInitBattlesList()");

	//Main.osgi.registerService(IBattleSelectModelBase, this->battleSelect.battleSelectModel);

	IMain *main = Service::getIMain();
	main->showLobby();
	main->clearBattles();

	QJsonDocument doc = QJsonDocument::fromJson(json);
	QJsonObject obj = doc.object();
	QJsonArray json_items = obj.value("items").toArray();
	QJsonArray json_battles = obj.value("battles").toArray();

	for (const QJsonValue &v : json_items)
		//foreach (const QJsonValue &v, json_items)
	{
		QJsonObject obj1 = v.toObject();
		MapClient map;
		map.ctf = obj1.value("ctf").toBool();
		map.gameName = obj1.value("gameName").toString();
		map.id = obj1.value("id").toString();
		map.maxPeople = obj1.value("maxPeople").toInt();
		map.maxRank = obj1.value("maxRank").toInt();
		map.minRank = obj1.value("minRank").toInt();
		map.name = obj1.value("name").toString();
		map.previewId = obj1.value("id").toString() + "_preview";
		map.tdm = obj1.value("tdm").toBool();
		map.dom = obj1.value("dom").toBool();
		map.hr = obj1.value("hr").toBool();
		map.themeName = obj1.value("themeName").toString();
		//maps.append(map);
	}


	for (const QJsonValue &v : json_battles)
		//foreach (const QJsonValue &v, json_battles)
	{
		QJsonObject btl = v.toObject();
		BattleClient battle;
		battle.battleId = btl.value("battleId").toString();
		battle.mapId = btl.value("mapId").toString();
		battle.name = btl.value("name").toString();
		battle.team = btl.value("team").toBool();
		battle.countRedPeople = btl.value("redPeople").toInt();
		battle.countBluePeople = btl.value("bluePeople").toInt();
		battle.countPeople = btl.value("countPeople").toInt();
		battle.maxPeople = btl.value("maxPeople").toInt();
		battle.minRank = btl.value("minRank").toInt();
		battle.maxRank = btl.value("maxRank").toInt();
		battle.paid = btl.value("isPaid").toBool();
		main->addBattle(battle);
	}


	//this->battleSelect.battleSelectModel.initObject(null, 10, js.haveSubscribe, maps);
	//this->battleSelect.battleSelectModel.initBattleList(null, battles, js.recommendedBattle, false);
	if (this->listInited == false)
		this->listInited = true;
}


void Lobby::parseBattleInfo(const QByteArray &json)
{
	QJsonDocument doc = QJsonDocument::fromJson(json);
	QJsonObject obj = doc.object();

	if (obj.value("null_battle").toBool())
		return;

	QJsonArray users_in_battle = obj.value("users_in_battle").toArray();

	IMain *main = Service::getIMain();


	ShowBattleInfo showBattleInfo;
	showBattleInfo.name = obj.value("name").toString();
	showBattleInfo.maxPeople = obj.value("maxPeople").toInt();
	showBattleInfo.type = BattleType_getType(obj.value("type").toString());
	showBattleInfo.battleId = obj.value("battleId").toString();
	showBattleInfo.previewId = obj.value("previewId").toString();
	showBattleInfo.minRank = obj.value("minRank").toInt();
	showBattleInfo.maxRank = obj.value("maxRank").toInt();
	showBattleInfo.timeLimit = obj.value("timeLimit").toInt();
	showBattleInfo.timeCurrent = obj.value("timeCurrent").toInt();
	showBattleInfo.killsLimit = obj.value("killsLimit").toInt();
	showBattleInfo.scoreRed = obj.value("scoreRed").toInt();
	showBattleInfo.scoreBlue = obj.value("scoreBlue").toInt();
	showBattleInfo.autobalance = obj.value("autobalance").toBool();
	showBattleInfo.frielndyFie = obj.value("frielndyFie").toBool();
	showBattleInfo.paidBattle = obj.value("paidBattle").toBool();
	showBattleInfo.withoutBonuses = obj.value("withoutBonuses").toBool();
	showBattleInfo.userAlreadyPaid = obj.value("userAlreadyPaid").toBool();
	showBattleInfo.fullCash = obj.value("fullCash").toBool();

	//((!(obj.clanName == null)) ? obj.clanName : "") : obj.spectator

	//var user_obj:Object;
	//var usr:UserInfoClient;
	//var obj:Object = JSON.parse(json);
	//var users:Array = new Array();

	for (const QJsonValue &v : users_in_battle)
		//foreach (const QJsonValue &v, users_in_battle)
	{
		QJsonObject user_obj = v.toObject();
		UserInfoClient usr;
		usr.id = user_obj.value("nickname").toString();
		usr.kills = user_obj.value("kills").toInt();
		usr.name = usr.id;
		usr.rank = user_obj.value("rank").toInt();
		usr.type = BattleTeamType_getType(user_obj.value("team_type").toString());
		showBattleInfo.users.append(usr);
	}

	main->showBattleInfo(showBattleInfo);

	//this->battleSelect.battleSelectModel.showBattleInfo(null, obj.name, obj.maxPeople, BattleType.getType(obj.type), obj.battleId, obj.previewId, obj.minRank, obj.maxRank, obj.timeLimit, obj.timeCurrent, obj.killsLimit, obj.scoreRed, obj.scoreBlue, obj.autobalance, obj.frielndyFie, users, obj.paidBattle, obj.withoutBonuses, obj.userAlreadyPaid, obj.fullCash, ((!(obj.clanName == null)) ? obj.clanName : ""), obj.spectator);
	//this->battleSelect.battleSelectModel.selectedBattleId = obj.battleId;
}



BattleType BattleModeConvertToBattleType(BattleMode mode)
{
	switch (mode)
	{
		case BattleMode::DM:
			return BattleType::DM;
		case BattleMode::TDM:
			return BattleType::TDM;
		case BattleMode::CTF:
			return BattleType::CTF;
		case BattleMode::CP:
			return BattleType::DOM;
		default:
			return BattleType::DM;
	};
}

void Lobby::parseBattleInfo_2(const QByteArray &json)
{
	QJsonDocument doc = QJsonDocument::fromJson(json);
	QJsonObject parser = doc.object();

	//var battleDMInfoCC:_SafeStr_38;
	//var user:*;
	//var _local_10:_SafeStr_10;
	//var _local_11:*;
	//var _local_12:*;

	BattleMode battleMode = BattleModeFromString(parser.value("battleMode").toString());

	/*
	var gameClass:IGameClass = gameTypeRegistry.getClass(Long.getLong(5823622, 5812058));
	if (this._SafeStr_4981(parser.battleMode) != BattleMode.DM)
	{
		gameClass = gameTypeRegistry.getClass(Long.getLong(58236223, 58120559));
	}
	*/

	//var battleObject:IGameObject = this._SafeStr_4937.createObject(Long.getLong(this._SafeStr_4962++, this._SafeStr_4962++), gameClass, parser.itemId);
	BattleInfoCC *battleInfoCC = new BattleInfoCC();
	battleInfoCC->battleMode = battleMode;
	battleInfoCC->itemId = parser.value("itemId").toString();
	battleInfoCC->limits.scoreLimit = parser.value("scoreLimit").toInt();
	battleInfoCC->limits.timeLimitInSec = parser.value("timeLimitInSec").toInt();
	//battleInfoCC->map = this._SafeStr_4938.getObject(Long.getLong((parser.preview * 1000), (parser.preview * 1000)));
	battleInfoCC->maxPeopleCount = parser.value("maxPeopleCount").toInt();
	battleInfoCC->name = parser.value("name").toString();
	battleInfoCC->proBattle = parser.value("proBattle").toBool();
	battleInfoCC->rankRange.max = parser.value("maxRank").toInt();
	battleInfoCC->rankRange.min = parser.value("minRank").toInt();
	battleInfoCC->roundStarted = parser.value("roundStarted").toBool();
	battleInfoCC->spectator = parser.value("spectator").toBool();
	battleInfoCC->timeLeftInSec = parser.value("timeLeftInSec").toInt();
	battleInfoCC->userPaidNoSuppliesBattle = parser.value("userPaidNoSuppliesBattle").toBool();
	battleInfoCC->withoutBonuses = parser.value("withoutBonuses").toBool();
	battleInfoCC->withoutCrystals = parser.value("withoutCrystals").toBool();
	battleInfoCC->withoutSupplies = parser.value("withoutSupplies").toBool();
	battleInfoCC->withoutUpgrades = true;

	//ProBattleCC *proBattleCC = new ProBattleCC();
	//proBattleCC->proBattleEnterPrice = parser.proBattleEnterPrice;
	//int proBattleTimeLeftInSec = parser.value("proBattleTimeLeftInSec");
	//proBattleCC->timeLeftInSec = proBattleTimeLeftInSec == -1 ? 0 : proBattleTimeLeftInSec;


	//Model.object = battleObject;
	//_SafeStr_19(modelRegistry.getModel(battleObject.gameClass.models[2])).putInitParams(proBattleCC);
	//_SafeStr_19(modelRegistry.getModel(battleObject.gameClass.models[2])).objectLoadedPost();
	//BattleInfoModel(modelRegistry.getModel(battleObject.gameClass.models[0])).putInitParams(battleInfoCC);




	IMain *main = Service::getIMain();


	ShowBattleInfo showBattleInfo;
	showBattleInfo.name = battleInfoCC->name;
	showBattleInfo.maxPeople = battleInfoCC->maxPeopleCount;
	showBattleInfo.type = BattleModeConvertToBattleType(battleMode);
	showBattleInfo.battleId = battleInfoCC->itemId;
	showBattleInfo.previewId = ""; //obj.value("previewId").toString();
	showBattleInfo.minRank = battleInfoCC->rankRange.min;
	showBattleInfo.maxRank = battleInfoCC->rankRange.max;
	showBattleInfo.timeLimit = battleInfoCC->limits.timeLimitInSec;
	showBattleInfo.timeCurrent = 0; //obj.value("timeCurrent").toInt();
	showBattleInfo.killsLimit = 0; //obj.value("killsLimit").toInt();
	showBattleInfo.scoreRed = 0; //obj.value("scoreRed").toInt();
	showBattleInfo.scoreBlue = 0; //obj.value("scoreBlue").toInt();
	showBattleInfo.autobalance = false; //obj.value("autobalance").toBool();
	showBattleInfo.frielndyFie = false; //obj.value("frielndyFie").toBool();
	showBattleInfo.paidBattle = false; //obj.value("paidBattle").toBool();
	showBattleInfo.withoutBonuses = battleInfoCC->withoutBonuses;
	showBattleInfo.userAlreadyPaid = false; //obj.value("userAlreadyPaid").toBool();
	showBattleInfo.fullCash = false; //obj.value("fullCash").toBool();



	//BattleInfoModel(modelRegistry.getModel(battleObject.gameClass.models[0])).objectLoaded();
	if (battleMode == BattleMode::DM)
	{
		//BattleDMInfoCC *battleDMInfoCC = new BattleDMInfoCC();
		QJsonArray users = parser.value("users").toArray();
		for (int i = 0; i < users.count(); i++)
		{
			QJsonObject user = users.at(i).toObject();
			//BattleInfoUser *battleInfoUser = new BattleInfoUser();
			//battleInfoUser->kills = user.value("kills").toInt();
			//battleInfoUser->score = user.value("score").toInt();
			//battleInfoUser->suspicious = user.value("suspicious").toBool();
			//battleInfoUser->user = user.value("user").toString();
			//battleDMInfoCC->users.append(battleInfoUser);

			UserInfoClient usr;
			usr.id = user.value("user").toString();
			usr.kills = user.value("kills").toInt();
			usr.name = usr.id;
			usr.rank = 1;
			usr.type = BattleTeamType::NONE;
			showBattleInfo.users.append(usr);
		}

		//BattleDmInfoModel(modelRegistry.getModel(battleObject.gameClass.models[1])).putInitParams(battleDMInfoCC);
		//BattleDmInfoModel(modelRegistry.getModel(battleObject.gameClass.models[1])).objectLoadedPost();
	}
	else
	{
		TeamBattleInfoCC *teamBattleInfoCC = new TeamBattleInfoCC();
		teamBattleInfoCC->autoBalance = parser.value("autoBalance").toBool();
		teamBattleInfoCC->friendlyFire = parser.value("friendlyFire").toBool();
		teamBattleInfoCC->scoreBlue = parser.value("scoreBlue").toInt();
		teamBattleInfoCC->scoreRed = parser.value("scoreRed").toInt();

		QJsonArray usersBlue = parser.value("usersBlue").toArray();
		QJsonArray usersRed = parser.value("usersRed").toArray();

		for (int i = 0; i < usersBlue.count(); i++)
		{
			QJsonObject user = usersBlue.at(i).toObject();
			//BattleInfoUser *blueInfo = new BattleInfoUser();
			//blueInfo.kills = user.value("kills");
			//blueInfo.score = user.value("score");
			//blueInfo.suspicious = user.value("suspicious");
			//blueInfo.user = user.value("user");
			//teamBattleInfoCC->usersBlue.append(blueInfo);

			UserInfoClient usr;
			usr.id = user.value("user").toString();
			usr.kills = user.value("kills").toInt();
			usr.name = usr.id;
			usr.rank = 1;
			usr.type = BattleTeamType::BLUE;
			showBattleInfo.users.append(usr);
		}

		for (int i = 0; i < usersRed.count(); i++)
		{
			QJsonObject user = usersRed.at(i).toObject();
			//BattleInfoUser *redInfo = new BattleInfoUser();
			//redInfo.kills = user.value("kills");
			//redInfo.score = user.value("score");
			//redInfo.suspicious = user.value("suspicious");
			//redInfo.user = user.value("user");
			//teamBattleInfoCC->usersRed.append(redInfo);

			UserInfoClient usr;
			usr.id = user.value("user").toString();
			usr.kills = user.value("kills").toInt();
			usr.name = usr.id;
			usr.rank = 1;
			usr.type = BattleTeamType::RED;
			showBattleInfo.users.append(usr);
		}
		//_SafeStr_4(modelRegistry.getModel(battleObject.gameClass.models[1])).putInitParams(teamBattleInfoCC);
		//_SafeStr_4(modelRegistry.getModel(battleObject.gameClass.models[1])).objectLoadedPost();
	}
	//Model.popObject();

	main->showBattleInfo(showBattleInfo);
}

void Lobby::parseBattle(const QByteArray &json)
{
	QJsonDocument doc = QJsonDocument::fromJson(json);
	QJsonObject obj = doc.object();

	BattleClient battle;
	battle.battleId = obj.value("battleId").toString();
	battle.mapId = obj.value("mapId").toString();
	battle.name = obj.value("name").toString();
	battle.team = obj.value("team").toBool();
	battle.countRedPeople = obj.value("redPeople").toInt();
	battle.countBluePeople = obj.value("bluePeople").toInt();
	battle.countPeople = obj.value("countPeople").toInt();
	battle.maxPeople = obj.value("maxPeople").toInt();
	battle.minRank = obj.value("minRank").toInt();
	battle.maxRank = obj.value("maxRank").toInt();
	battle.paid = obj.value("isPaid").toBool();
	Service::getIMain()->addBattle(battle);
}




void Lobby::parseBattle_2(QJsonObject &parser)
{
	IMain *main = Service::getIMain();

	/*
	//var gameClass:IGameClass = gameTypeRegistry.getClass(Long.getLong(5823623, 5812059));
	if (this._SafeStr_4981(parser.battleMode) != BattleMode.DM)
	{
		gameClass = gameTypeRegistry.getClass(Long.getLong(58236221, 58120558));
	}
	if (this._SafeStr_4938.getObjectByName(parser.battleId) != null)
	{
		this._SafeStr_4938.destroyObject(this._SafeStr_4938.getObjectByName(parser.battleId).id);
	}
	*/

	BattleMode battleMode = BattleModeFromString(parser.value("battleMode").toString());

	//IGameObject *battleObject = this._SafeStr_4938.createObject(Long.getLong((this._SafeStr_4962++ * 100), (this._SafeStr_4962++ * 100)), gameClass, parser.battleId);
	BattleItemCC *battleItemCC = new BattleItemCC();
	battleItemCC->battleId = parser.value("battleId").toString();
	battleItemCC->battleMode = battleMode;
	//battleItemCC->map = this._SafeStr_4938.getObject(Long.getLong((parser.preview * 1000), (parser.preview * 1000)));
	battleItemCC->maxPeople = parser.value("maxPeople").toInt();
	battleItemCC->name = parser.value("name").toString();
	battleItemCC->privateBattle = parser.value("privateBattle").toBool();
	battleItemCC->proBattle = parser.value("proBattle").toBool();
	battleItemCC->rankRange.max = parser.value("maxRank").toInt();
	battleItemCC->rankRange.min = parser.value("minRank").toInt();
	battleItemCC->suspicious = parser.value("suspicious").toBool();


	BattleClient battle;
	battle.battleId = battleItemCC->battleId;
	battle.mapId = "";
	battle.name = battleItemCC->name;
	battle.team = battleMode != BattleMode::DM;
	battle.maxPeople = battleItemCC->maxPeople;
	battle.minRank = battleItemCC->rankRange.min;
	battle.maxRank = battleItemCC->rankRange.max;
	battle.paid = false;
	battle.countRedPeople = 0;
	battle.countBluePeople = 0;
	battle.countPeople = 0;


	delete battleItemCC;


	//Model.object = battleObject;

	//BattleItemModel(modelRegistry.getModel(battleObject.gameClass.models[0])).putInitParams(battleItemCC);

	if (battleMode == BattleMode::DM)
	{

		BattleDMItemCC *battleDMItemCC = new BattleDMItemCC();

		QJsonArray users = parser.value("users").toArray();
		for (int i = 0; i < users.count(); i++)
			battleDMItemCC->users.append(users.at(i).toString());

		battle.countPeople = users.count();

		//BattleDMItemModel(modelRegistry.getModel(battleObject.gameClass.models[1])).putInitParams(battleDMItemCC);
		//BattleDMItemModel(modelRegistry.getModel(battleObject.gameClass.models[1])).objectLoaded();
	}
	else
	{

		TeamBattleItemCC *teamBattleItemCC = new TeamBattleItemCC();

		QJsonArray usersBlue = parser.value("usersBlue").toArray();
		for (int i = 0; i < usersBlue.count(); i++)
			teamBattleItemCC->usersBlue.append(usersBlue.at(i).toString());

		QJsonArray usersRed = parser.value("usersRed").toArray();
		for (int i = 0; i < usersRed.count(); i++)
			teamBattleItemCC->usersRed.append(usersRed.at(i).toString());

		battle.countBluePeople = usersBlue.count();
		battle.countRedPeople = usersRed.count();

		//BattleTeamItemModel(modelRegistry.getModel(battleObject.gameClass.models[1])).putInitParams(teamBattleItemCC);
		//BattleTeamItemModel(modelRegistry.getModel(battleObject.gameClass.models[1])).objectLoaded();
	}

	main->addBattle(battle);

	//BattleItemModel(modelRegistry.getModel(battleObject.gameClass.models[0])).objectLoadedPost();
	//this._SafeStr_4967.push(battleObject);

}

void Lobby::parseAndInitBattlesList_2(const QByteArray &json)
{
	IMain *main = Service::getIMain();
	main->showLobby();
	main->clearBattles();

	/*
	var battle:*;
	var models:Vector.<Long> = new Vector.<Long>();
	models.push(Long.getLong(0, 300090008));
	models.push(Long.getLong(0, 300090004));
	gameTypeRegistry.createClass(Long.getLong(5823623, 5812059), models);
	models = new Vector.<Long>();
	models.push(Long.getLong(0, 300090007));
	models.push(Long.getLong(0, 300090003));
	models.push(Long.getLong(0, 300090023));
	gameTypeRegistry.createClass(Long.getLong(5823622, 5812058), models);
	models = new Vector.<Long>();
	models.push(Long.getLong(0, 300090008));
	models.push(Long.getLong(0, 300090028));
	gameTypeRegistry.createClass(Long.getLong(58236221, 58120558), models);
	models = new Vector.<Long>();
	models.push(Long.getLong(0, 300090007));
	models.push(Long.getLong(0, 300090027));
	models.push(Long.getLong(0, 300090023));
	gameTypeRegistry.createClass(Long.getLong(58236223, 58120559), models);
	trace("init battle select", json);
	*/

	//this.battleSelectModel.objectLoadedPost();

	QJsonDocument doc = QJsonDocument::fromJson(json);
	QJsonObject parser = doc.object();
	QJsonArray battles = parser.value("battles").toArray();
	for (int i = 0; i < battles.count(); i++)
	{
		QJsonObject obj = battles.at(i).toObject();
		this->parseBattle_2(obj);
	}

	//this.battleSelectModel.battleItemsPacketJoinSuccess();
	//LobbyLayoutNotifyModel(OSGi.getInstance().getService(ILobbyLayoutNotify)).cancelPredictedLayoutSwitch();


	if (this->listInited == false)
		this->listInited = true;
}


void Lobby::parseGarageItems(const QByteArray &json, const QString &src)
{
	//ResourceUtil::getResource(ResourceType::IMAGE, "garage_box_img") == nullptr)) || ResourceUtil::getResource(ResourceType::IMAGE, "zeus_m0_preview") == nullptr)

	this->initGarageItems(json, src);

}


void Lobby::initGarageItems(const QByteArray &json, const QString &src)
{
	(void)src;
	//var prop:Object;


	this->garage->garageModel->initObject(nullptr, "russia", 1000000, Long(100, 100), Service::getNetworker());


	QJsonDocument doc = QJsonDocument::fromJson(json);

	QJsonObject parser = doc.object();

	QJsonArray json_items = parser.value("items").toArray();

	for (int i = 0; i < json_items.count(); i++)
	{
		QJsonObject obj = json_items.at(i).toObject();
		QVector<std::shared_ptr<ModificationInfo>> modifications;

		QJsonArray json_modification = obj.value("modification").toArray();

		QVector<ItemPropertyValue> props;

		for (int i = 0; i < json_modification.count(); i++)
		{
			QJsonObject obj2 = json_modification.at(i).toObject();

			QJsonArray json_properts = obj2.value("properts").toArray();

			for (int i = 0; i < json_properts.count(); i++)
			{
				QJsonObject prop = json_properts.at(i).toObject();

				ItemPropertyValue p;
				p.property = ItemPropertyFromString(prop.value("property").toString());
				p.value = prop.value("value").toString();
				props.append(p);
			}

			std::shared_ptr<ModificationInfo> info = std::make_shared<ModificationInfo>();
			info->crystalPrice = obj2.value("price").toInt();
			info->rankId = obj2.value("rank").toInt();
			info->previewId = obj2.value("previewId").toString();
			//QString pid = obj2.previewId;
			info->itemProperties = props;
			modifications.append(info);
		}

		int id = obj.value("modificationID").toInt();
		std::shared_ptr<ItemParams> item = std::make_shared<ItemParams>(obj.value("id").toString() + "_m" + QString::number(id),
																		obj.value("description").toString(),
																		obj.value("isInventory").toBool(),
																		obj.value("index").toInt(),
																		props,
																		ItemTypeEnumFromInt(obj.value("type").toInt()),
																		obj.value("modificationID").toInt(),
																		obj.value("name").toString(),
																		obj.value("next_price").toInt(),
																		nullptr,
																		obj.value("next_rank").toInt(),
																		modifications[id]->previewId,
																		obj.value("price").toInt(),
																		obj.value("rank").toInt(),
																		modifications);

		if (item->baseItemId == "smoky_xt_m0")
		{
			//trace(item.baseItemId);
		}

		std::shared_ptr<ItemInfo> infoItem = std::make_shared<ItemInfo>();
		/*
		if (this->getTypeItem(obj.type) != ItemTypeEnum::ARMOR)
		{
			if (this->getTypeItem(obj.type) != ItemTypeEnum::WEAPON)
			{
				if (this->getTypeItem(obj.type) == ItemTypeEnum::COLOR)
				{
				}
			}
		}
		*/

		infoItem->count = obj.value("count").toInt();
		infoItem->itemId = item->baseItemId;
		this->garage->garageModel->initItem(item->baseItemId, item);

		QVector<std::shared_ptr<ItemInfo>> itemsOnSklad;
		itemsOnSklad.append(infoItem);
		this->garage->garageModel->initDepot(nullptr, itemsOnSklad);
	}


	network->send("garage;get_garage_data");
	//PanelModel(Main.osgi.getService(IPanel)).addListener(this->garage.garageModel);
	//Main.osgi.registerService(IGarage, this->garage.garageModel);
	//PanelModel(Main.osgi.getService(IPanel)).isGarageSelect = true;

	//catch(e:Error)
	//throw (new Error(("Ошибка " + e.getStackTrace())));
}

