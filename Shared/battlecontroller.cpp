#include "battlecontroller.h"
#include "networking/commands/command.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/TankSpecification.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/tankparts.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/ClientTank.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/tanks/model/panel/panelmodel.h"
#include "alternativa/object/clientobject.h"
#include "alternativa/object/clientobjectweapon.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/weapon/shared/shot/shotdata.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/models/weapon/gun/smokymodel.h"
#include "alternativa/tanks/models/weapon/weaponmodelempty.h"
#include "alternativa/tanks/models/weapon/flamethrower/flamethrowermodel.h"
#include "alternativa/tanks/models/weapon/weakening/weaponweakeningmodel.h"
#include "service.h"
#include "IMain.h"
#include "networking/network.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/deathreason.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/ctf/ctfmodel.h"
#include "alternativa/tanks/models/ctf/ClientFlag.h"
#include "alternativa/tanks/models/ctf/FlagsState.h"
#include "alternativa/object/clientobjectctf.h"
#include "battlechatmessage.h"
#include "alternativa/tanks/models/dom/dommodel.h"
#include "alternativa/tanks/models/dom/server/DOMPointData.h"
#include "alternativa/object/clientobjectbonus.h"
#include "alternativa/tanks/models/battlefield/mine/battleminesmodel.h"
#include "server/models/mines/serverbattleminesmodel.h"
#include "alternativa/tanks/models/effects/common/bonuscommon/BonusCommonData.h"
#include "alternativa/init/battlefieldsharedactivator.h"
#include "alternativa/tanks/models/weapon/plasma/plasmamodel.h"
#include "alternativa/tanks/models/battlefield/effects/damageeffect.h"
#include "alternativa/tanks/models/weapon/thunder/thundermodel.h"
#include "server/models/inventory/serverinventorymodel.h"
#include "server/models/inventory/ServerInventoryData.h"
#include "projects/tanks/client/battlefield/gui/models/effectsvisualization/battleeffect.h"
#include "alternativa/tanks/models/effectsvisualization/effectsvisualizationmodel.h"
#include "alternativa/tanks/models/weapon/healing/healinggunmodel.h"
#include "com/alternativaplatform/projects/tanks/client/warfare/models/tankparts/weapon/healing/struct/IsisAction.h"
#include "alternativa/tanks/models/sfx/lightdatamanager.h"
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantMap>
#include "__global.h"
#include "alternativa/tanks/models/weapon/shaft/shaftmodel.h"
#include "server/models/shaft/ServerShaftTargetData.h"
#include "alternativa/tanks/models/weapon/freeze/freezemodel.h"
#include "alternativa/tanks/models/weapon/railgun/railgunmodel.h"
#include "alternativa/tanks/models/weapon/terminator/terminatormodel.h"
#include "alternativa/tanks/models/weapon/ricochet/ricochetmodel.h"
#include <QTimer>
#include "alternativa/tanks/models/battlefield/gamemode/gamemodes.h"
#include "pro/RotateTurretCommand.h"

#ifdef L_GT
#include "botmodel.h"
#endif


static void readVector3(const QJsonObject &o, Vector3 &v)
{
	_function_name("BattleController::readVector3");

	v.x = o.value("x").toDouble();
	v.y = o.value("y").toDouble();
	v.z = o.value("z").toDouble();
}



bool BattleController::onData(Command *command)
{
	_function_name("BattleController::onData");

	if (command->type != CommandType::BATTLE)
		return false;

	QByteArray cmd = command->readArgument();

	if (battle_init == false) {
		if (cmd == "init_shots_data") {
#ifndef L_PRO
			this->parseShotsData(command->readRest());
#else
			QByteArray json = command->readArgument();
			this->parseShotsData(json);
#endif
		} else if (cmd == "init_battle_model") {
			this->initBattle(command->readRest());
#ifdef L_GT
		} else if (cmd == "init_turret_sfx_lighting") {
#else
		} else if (cmd == "init_sfx_data") {
#endif
#ifdef GRAPHICS
			LightDataManager::init(command->readRest());
#endif
		}

		return true;
	}

	if (cmd == "move") {
		this->moveTank(command->readRest());
#ifdef L_GT
	} else if (cmd == "move_bot") {
		this->moveBot(command->readRest());
#endif
#ifdef L_PRO
	} else if (cmd == "movementControl") {
		this->movementControl(command->readRest());

	} else if (cmd == "rotateTurret") {
		const QByteArray data = command->readRest();
		this->rotateTurret(data);
#endif

	} else if (cmd == "init_gui_model") {
		/*
		parser = JSON.parse(data.args[1]);
		battle = new BattleStatInfo();
		battle->blueScore = parser.score_blue;
		battle->fund = parser.fund;
		battle->redScore = parser.score_red;
		battle->scoreLimit = parser.scoreLimit;
		battle->teamPlay = parser.team;
		battle->timeLeft = parser.currTime;
		battle->timeLimit = parser.timeLimit;
		PanelModel(Main.osgi.getService(IPanel)).isInBattle = true;
		users = new Array();
		StatisticsModel(Main.osgi.getService(IBattlefieldGUI)).initObject(null, parser.name);
		i = 0;
		for each (obj in parser.users)
		{
			users[i] = new UserStat(0, 0, obj.nickname, obj.rank, 0, 0, BattleTeamType.getType(obj.teamType), obj.nickname);
			i = i + 1;
		}
		StatisticsModel(Main.osgi.getService(IBattlefieldGUI)).init(new ClientObject("bfObject", null, "bfObject", null), battle, users);
		ChatModel(Main.osgi.getService(IChatBattle)).objectLoaded(null);
		*/
		this->serverMinesModel->init();
		//CheatManager.getInstance().addEventListener(CheatManagerEvent.CHEAT_DETECTION, Game.onUserEntered);
		//this->serverInventoryModel->init();

	} else if (cmd == "init_tank") {
		this->initTank(command->readRest());
	} else if (cmd == "create_levelup_effect") {
		const QString userId = command->readArgument();
		int rang = command->readRest().toInt();
		tankModel->createLevelUpEffect(userId, rang);
	} else if (cmd == "create_critical_hit_effect") {
		tankModel->createCriticalHitEffect(command->readRest());
	} else if (cmd == "gold_spawn") {

	} else if (cmd == "ruby_spawn") {

	} else if (cmd == "update_spectator_list") {

	} else if (cmd == "activate_tank") {
		Service::getTankModel()->activateTank(tanks->value(command->readArgument()));
	} else if (cmd == "kill_tank") {

		QString id = command->readArgument();
		ClientObjectUser *user = tanks->value(id);
		if (user != nullptr)
		{
			QString reason = command->readArgument();
			QString killerTankId = command->readArgument();
			Service::getTankModel()->kill(user, DeathReason_fromString(reason), killerTankId);
		}


	} else if (cmd == "prepare_to_spawn") {

		QString id = command->readArgument();


		if (tanks->contains(id))
		{
			//-1824.021@-3360.458@0.0@10.996000289916992

			QList<QByteArray> tempArr = command->readRest().split('@');

			TankModel *tank_model = Service::getTankModel();
			ClientObjectUser *user = tanks->value(id);
			if (user == nullptr)
				throw 6324123;

			if (tempArr.length() == 4)
			{
				Vector3 pos(tempArr[0].toDouble(), tempArr[1].toDouble(), tempArr[2].toDouble());
				Vector3 orientation(0, 0, tempArr[3].toDouble());
				tank_model->prepareToSpawn(user, &pos, &orientation);
#ifdef L_PRO
				QTimer::singleShot(5000, new QObject(), [this] {
					this->network->send("battle;readyToPlace");
				});
#endif
			}
		}

	} else if (cmd == "spawn") {
		//this->parseSpawnCommand(command->readArgument());
		this->parseSpawnCommand(command->readRest());
	} else if (cmd == "move") {

	} else if (cmd == "chat") {
		this->onChatMessage(command->readRest());
	} else if (cmd == "spectator_message") {
		this->onChatMessageSpectator(command->readRest());
	} else if (cmd == "remove_user") {
		this->removeUser(command->readRest());
	} else if (cmd == "ping") {

	} else if (cmd == "tracePing") {

	} else if (cmd == "tank_smooth") {

	} else if (cmd == "spawn_bonus") {
		this->parseSpawnBonus(command->readRest());
	} else if (cmd == "take_bonus_by") {
		battle->bonusTaken(nullptr, command->readRest());
	} else if (cmd == "user_log") {

	} else if (cmd == "set_cry") {
		PanelModel *modelPanel = Service::getPanelModel();
		int crystal = command->readArgument().toInt();
		modelPanel->updateCrystal(nullptr, crystal);
	} else if (cmd == "remove_bonus") {
		battle->removeBonus(nullptr, command->readRest());
	} else if (cmd == "start_fire") {
		QString user_id = command->readArgument();
		ClientObjectUser *user = tanks->value(user_id);
		QByteArray data = command->readRest();
		this->parseStartFire(user, user_id, data);
	} else if (cmd == "fire") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		QByteArray js = command->readRest();
		this->parseFire(user, js);
	} else if (cmd == "change_health") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		int newHealth = command->readRest().toInt();
		tankModel->changeHealth(user, newHealth);
#ifdef L_MT
	} else if (cmd == "damage_tank") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		double damage = command->readArgument().toDouble();

		uint32_t color;

		QByteArray s = command->readArgument();
		if (s == "FATAL") {
			color = 0xF03416;
		} else if (s == "CRITICAL") {
			color = 0xD8C90F;
		} else if (s == "HEAL") {
			color = 0x5D118;
		} else {
			color = 0xFFFFFF;
		}

		this->createDamageEffect(user, damage, color);
#endif


#ifdef L_OT
	} else if (cmd == "hp") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		double damage = command->readArgument().toDouble();
		double color_id = command->readRest().toInt();
		this->parseHP(user, damage, color_id);
#endif


	} else if (cmd == "stop_fire") {
		QString user_id = command->readArgument();
		ClientObjectUser *user = tanks->value(user_id);
		this->parseStopFire(user, user_id);
	} else if (cmd == "kick_for_cheats") {

	} else if (cmd == "update_player_statistic") {

	} else if (cmd == "change_fund") {

	} else if (cmd == "battle_finish") {
		this->parseFinishBattle(command->readRest());
	} else if (cmd == "battle_restart") {

		//StatisticsModel(Main.osgi.getService(IBattlefieldGUI)).restart(null, data.args[1]);
		battle->battleRestart();
	} else if (cmd == "start_fire_twins") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		this->parseStartFireTwins(user, command->readRest());
	} else if (cmd == "start_fire_terminator") {

	} else if (cmd == "start_fire_snowman") {

	} else if (cmd == "show_nube_up_score") {

	} else if (cmd == "show_nube_new_rank") {

	} else if (cmd == "change_spec_tank") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		this->parseChangeSpecTank(user, command->readRest());
	} else if (cmd == "change_temperature_tank") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		tankModel->setTemperature(user, command->readRest().toDouble());
	} else if (cmd == "fire_ricochet") {

	} else if (cmd == "change_team_scores") {

	} else if (cmd == "init_ctf_model") {
		parseCTF(command->readRest());
	} else if (cmd == "init_dom_model") {
		parseDOM(command->readRest());
	} else if (cmd == "flagTaken") {
		if (ctfModel != nullptr) {
			QString tankId = command->readArgument();
			QString team = command->readArgument();
			ctfModel->flagTaken(nullptr, tankId, BattleTeamType_getType(team));
		}
	} else if (cmd == "deliver_flag") {
		QString team = command->readArgument();
		QString tankId = command->readArgument();
		deliverFlag(tankId, team);
	} else if (cmd == "flag_drop") {
		flagDrop(command->readRest());
	} else if (cmd == "return_flag") {
		QString team = command->readArgument();
		QString tankId = command->readArgument();
		returnFlag(tankId, team);
	} else if (cmd == "show_warning_table") {

	} else if (cmd == "change_user_team") {

	} else if (cmd == "init_inventory") {
#ifdef GRAPHICS
		QJsonDocument document = QJsonDocument::fromJson(command->readAll());
		QJsonObject json = document.object();
		QJsonArray json_items = json.value("items").toArray();
		QVector<ServerInventoryData*> items;
		for (int i = 0; i < json_items.count(); i++)
		{
			QJsonObject item = json_items.at(i).toObject();
			ServerInventoryData *_item = new ServerInventoryData();
			_item->count = item.value("count").toInt();
			_item->id = item.value("id").toString();
			_item->itemEffectTime = item.value("itemEffectTime").toInt();
			_item->itemRestSec = item.value("itemRestSec").toInt();
			_item->slotId = item.value("slotId").toInt();
			items.append(_item);
		}
		this->serverInventoryModel->initInventory(items);
		qDeleteAll(items);
#endif
	} else if (cmd == "update_inventory") {
#ifdef GRAPHICS
		QJsonDocument document = QJsonDocument::fromJson(command->readAll());
		QJsonObject json = document.object();
		QJsonArray json_items = json.value("items").toArray();
		QVector<ServerInventoryData*> items;
		for (int i = 0; i < json_items.count(); i++)
		{
			QJsonObject item = json_items.at(i).toObject();
			ServerInventoryData *_item = new ServerInventoryData();
			_item->count = item.value("count").toInt();
			_item->id = item.value("id").toString();
			_item->itemEffectTime = item.value("itemEffectTime").toInt();
			_item->itemRestSec = item.value("itemRestSec").toInt();
			_item->slotId = item.value("slotId").toInt();
			items.append(_item);
		}
		this->serverInventoryModel->updateInventory(items);
		qDeleteAll(items);
#endif
	} else if (cmd == "activate_item") {
#ifdef GRAPHICS
		this->serverInventoryModel->activateItem(QString(command->readRest()));
#endif
	} else if (cmd == "enable_effect") {
#ifdef GRAPHICS
		ClientObjectUser *user = tanks->value(command->readArgument());
		int itemIndex = command->readArgument().toInt();
		int duration = command->readArgument().toInt();
		this->serverInventoryModel->enableEffect(user, itemIndex, duration);
#endif
	} else if (cmd == "disnable_effect") {
#ifdef GRAPHICS
		ClientObjectUser *user = tanks->value(command->readArgument());
		int itemIndex = command->readArgument().toInt();
		this->serverInventoryModel->disnableEffect(user, itemIndex);
#endif
	} else if (cmd == "init_effects") {
#ifdef GRAPHICS
		QJsonDocument document = QJsonDocument::fromJson(command->readAll());
		QJsonObject json = document.object();
		QJsonArray json_effects = json.value("effects").toArray();
		QVector<BattleEffect*> effects;
		for (int i = 0; i < json_effects.count(); i++)
		{
			QJsonObject obj = json_effects.at(i).toObject();
			BattleEffect *effect = new BattleEffect();
			effect->durationTime = obj.value("durationTime").toInt();
			effect->itemIndex = obj.value("itemIndex").toInt();
			effect->userID = obj.value("userID").toInt();
			effects.append(effect);
		}
		this->serverInventoryModel->enableEffects(nullptr, effects);
#endif
	} else if (cmd == "local_user_killed") {

	} else if (cmd == "init_mine_model") {
		const QByteArray data = command->readRest();
		this->serverMinesModel->initModel(data);
	} else if (cmd == "remove_mines") {
		const QString ownerId = command->readRest();
		this->serverMinesModel->removeMines(ownerId);
	} else if (cmd == "put_mine") {
		putMine(command->readRest());
	} else if (cmd == "activate_mine") {
		const QString ownerId = command->readRest();
		this->serverMinesModel->activateMine(ownerId);
	} else if (cmd == "hit_mine") {
		const QString mineId = command->readArgument();
		ClientObjectUser *user = tanks->value(command->readRest());
		this->serverMinesModel->hitMine(user, mineId);
	} else if (cmd == "init_mines") {
		const QByteArray data = command->readRest();
		this->serverMinesModel->initMines(data);
	} else if (cmd == "tank_capturing_point") {
		DomPointID pointId(command->readArgument());
		ClientObjectUser *user = tanks->value(command->readArgument());
		if (domModel != nullptr)
			domModel->serverTankCapturingPoint(pointId, user);
	} else if (cmd == "tank_leave_capturing_point") {
		ClientObjectUser *user = tanks->value(command->readArgument());
		DomPointID pointId(command->readArgument());
		if (domModel != nullptr)
			domModel->serverTankLeaveCapturingPoint(user, pointId);
	} else if (cmd == "set_point_score") {
		DomPointID pointId(command->readArgument());
		int score = command->readArgument().toInt();
		if (domModel != nullptr)
			domModel->serverSetPointScore(pointId, score);
	} else if (cmd == "point_captured_by") {
		const QString team = command->readArgument();
		DomPointID pointId(command->readArgument());
		if (domModel != nullptr)
			domModel->serverPointCapturedBy(team, pointId);
	} else if (cmd == "point_lost_by") {
		const QString team = command->readArgument();
		DomPointID pointId(command->readArgument());
		if (domModel != nullptr)
			domModel->serverPointLostBy(team, pointId);
	} else if (cmd == "update_point_predestal") {
		DomPointID pointId(command->readArgument());
		const QString team = command->readArgument();
		if (domModel != nullptr)
			domModel->updatePedestal(pointId, team);
	} else if (cmd == "shaft_quick_shot") {
		const QString userName = command->readArgument();
		const QByteArray json = command->readRest();
		this->parseShaftQuickFire(userName, json);

#ifdef L_GT
	} else if (cmd == "init_turret_sfx_lighting") {
#else
	} else if (cmd == "init_sfx_data") {
#endif
#ifdef GRAPHICS
		LightDataManager::init(command->readRest());
#endif

#ifdef L_OT
	} else if (cmd == "speedhack_detected") {
		this->moveAnticheatTank(command->readRest());
#endif
#ifdef L_GT
	} else if (cmd == "init_bot_tank") {
		botModel->addBot(command->readRest());
#endif

	} else {

	}


	return true;
}





BattleController::BattleController(Network *network)
{
	_function_name("BattleController::BattleController");

	this->network = network;
	this->battle_init = false;
	this->smokyModel = nullptr;
	this->flamethrowerModel = nullptr;
	this->weaponModelEmpty = nullptr;
	this->twinsModel = nullptr;
	this->thunderModel = nullptr;
	this->isidaModel = nullptr;
	this->shaftModel = nullptr;
	this->frezeeModel = nullptr;
	this->railgunModel = nullptr;
	this->terminatorModel = nullptr;
	this->ricochetModel = nullptr;


	client = nullptr;
	tanks = new QHash<QString, ClientObjectUser*>;


	this->battle = new BattlefieldModel();
	this->battleMinesModel = new BattleMinesModel;
	this->serverMinesModel = new ServerBattleMinesModel(this->battleMinesModel);

#ifdef GRAPHICS
	this->serverInventoryModel = new ServerInventoryModel();
	this->effectsModel = new EffectsVisualizationModel();
	this->serverInventoryModel->init(this->effectsModel);
#else
	this->serverInventoryModel = nullptr;
	this->effectsModel = nullptr;
#endif

	this->battle->battleMinesModel = this->battleMinesModel;

	Service::setIBattleField(this->battle);
	Service::setBattlefieldModel(this->battle);

	tankModel = new TankModel;
	tankModel->setBattlefield(this->battle);
	Service::setTankModel(tankModel);

	ctfModel = nullptr;
	domModel = nullptr;
	ctfObj = new ClientObjectCTF("ctfModel", nullptr, "ctfModelObj", nullptr);

	this->clientObjectBonuses = new QHash<QString, ClientObjectBonus*>;


	BattlefieldSharedActivator *shared = new BattlefieldSharedActivator();
	shared->start(nullptr); //Main.osgi

#ifdef L_GT
	botModel = new BotModel();
#else
	botModel = nullptr;


#endif




}

BattleController::~BattleController()
{
	delete tanks;

	network->removeListener(this);

	battleMinesModel->objectUnloaded(nullptr);

	battle->destroy();

	if (smokyModel != nullptr)
		delete smokyModel;
	if (flamethrowerModel != nullptr)
		delete flamethrowerModel;
	if (weaponModelEmpty != nullptr)
		delete weaponModelEmpty;
	if (twinsModel != nullptr)
		delete twinsModel;
	if (thunderModel != nullptr)
		delete thunderModel;
	if (isidaModel != nullptr)
		delete isidaModel;
	if (shaftModel != nullptr)
		delete shaftModel;
	if (frezeeModel != nullptr)
		delete frezeeModel;
	if (railgunModel != nullptr)
		delete railgunModel;
	if (terminatorModel != nullptr)
		delete terminatorModel;
	if (ricochetModel != nullptr)
		delete ricochetModel;

#ifdef GRAPHICS
	delete serverInventoryModel;
	delete effectsModel;
#endif

	Service::setDOMModel(nullptr);
	Service::setCTFModel(nullptr);
	Service::setTankModel(nullptr);
	Service::setBattleController(nullptr);


	QHashIterator<QString, ClientObjectBonus*> i(*this->clientObjectBonuses);
	while (i.hasNext()) {
		i.next();
		ClientObjectBonus *o = i.value();
		if (o->bonusData != nullptr)
			delete o->bonusData;
		delete o;
	}

	delete clientObjectBonuses;

	delete tankModel;
	WeaponsManager::clear();



#ifdef L_GT
	if (botModel != nullptr)
		delete botModel;
#endif

	if (ctfModel != nullptr)
		delete ctfModel;

	if (domModel != nullptr)
		delete domModel;

	delete ctfObj;
	delete battleMinesModel;
	delete serverMinesModel;

	if (client != nullptr)
		delete client;
}


void BattleController::destroy()
{
	_function_name("BattleController::destroy");
	delete this;
}

void BattleController::initTank(const QByteArray &js)
{
	_function_name("BattleController::initTank");
	try
	{
		QJsonDocument document = QJsonDocument::fromJson(js);
		QJsonObject json = document.object();

		TankParts tankParts;

		tankParts.coloringObjectId = json.value("colormap_id").toString();
		if (tankParts.coloringObjectId.isEmpty())
			tankParts.coloringObjectId = "green_m0";

		tankParts.hullObjectId = json.value("hull_id").toString();
		tankParts.turretObjectId = json.value("turret_id").toString();
#ifdef L_PRO
		if (tankParts.hullObjectId.startsWith("freeze_"))
		{
			tankParts.hullObjectId.replace("freeze_", "frezee_");
		}
#endif



		QString tank_id = json.value("tank_id").toString();

		int health = json.value("health").toInt();

		//ClientTank *clientTank = new ClientTank();
		ClientTank clientTank;


		TankState &state = clientTank.tankState;

		if (!json.value("state_null").toBool())
		{
			QStringList temp = json.value("position").toString().split(QChar('@'));
			if (temp.length() >= 4)
			{
				state.health = health;
				state.orientation.x = 0;
				state.orientation.y = 0;
				state.orientation.z = temp[3].toDouble();
				state.position.x = temp[0].toDouble();
				state.position.y = temp[1].toDouble();
				state.position.z = temp[2].toDouble();
			}
			else
			{
				state.health = 0;
				state.orientation.reset();
				state.position.reset();
			}
		} else {
			state.health = 0;
			state.orientation.reset();
			state.position.reset();
		}

		state.turretAngle = 0;
		state.control = 0;

		clientTank.health = health;
		clientTank.incarnationId = json.value("icration").toInt();
		clientTank.self = tank_id == client->getId();//tank_id == client.id;
		QString stateSpawn = json.value("state").toString();
		clientTank.spawnState = (stateSpawn == "newcome") ? TankSpawnState::NEWCOME : ((stateSpawn == "active") ? TankSpawnState::ACTIVE : ((stateSpawn == "suicide") ? TankSpawnState::SUICIDE : TankSpawnState::ACTIVE));
		clientTank.teamType = BattleTeamType_getType(json.value("team_type").toString());


		//123000.0,
		//9.09


#ifndef L_PRO
		clientTank.tankSpecification.speed = json.value("speed").toDouble();
#else
		clientTank.tankSpecification.speed = json.value("maxSpeed").toDouble();
#endif




		clientTank.tankSpecification.turnSpeed = json.value("turn_speed").toDouble();
		clientTank.tankSpecification.turretRotationSpeed = json.value("turret_turn_speed").toDouble();
		//this->myTank = clientTank;

		TankModel *tankModelService = Service::getTankModel();
		bool localTankInited = clientTank.self;

		std::shared_ptr<ClientObjectUser> user = this->initClientObject(tank_id, tank_id);
		tanks->insert(tank_id, user.get());

		const QString battleId = json.value("battleId").toString();
		const double mass = json.value("mass").toDouble();
		double power = json.value("power").toDouble();
		//const TankSoundScheme *soundScheme;
		//const TankParts *tankParts;
		//const TankResources *tankResources;
		const double impactForce = json.value("impact_force").toDouble();
		const double kickback = json.value("kickback").toDouble();
		const double turretRotationAcceleration = json.value("turret_rotation_accel").toDouble();
		const double turretRotationSpeed = json.value("turret_turn_speed").toDouble();
		const QString nickname = json.value("nickname").toString();
		const int rank = json.value("rank").toInt();

#ifdef L_PRO
		power = 123000.0;
#endif


		if (tankModelService->initObject(user, battleId, mass, power, nullptr, &tankParts,
										 nullptr, impactForce, kickback,
										 turretRotationAcceleration, turretRotationSpeed,
										 nickname, rank, tankParts.turretObjectId))
		{
			tankModelService->initTank(user, &clientTank, &tankParts, localTankInited);
		}

	}
	catch(int e)
	{
		qDebug() << "error initTank" << e;
	}

}


std::shared_ptr<ClientObjectUser> BattleController::initClientObject(const QString &id, const QString &name)
{
	_function_name("BattleController::initClientObject");
	//ClientClass *clientClass = new ClientClass(id, nullptr, name);
	return std::make_shared<ClientObjectUser>(id, name);
}



void BattleController::initBattle(const QByteArray &js)
{
	_function_name("BattleController::initBattle");

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();

	QString username = Service::getPanelModel()->getUserName();
	//var s:String;
	//this->initLocalClientObject(PanelModel(Main.osgi.getService(IPanel)).userName, PanelModel(Main.osgi.getService(IPanel)).userName);
	this->initLocalClientObject(username, username);
	//Object *json = JSON.parse(js);
	//BattlefieldSoundScheme soundsParams = new BattlefieldSoundScheme();
	//soundsParams.ambientSound = json.sound_id;
#ifdef GRAPHICS
	IGameMode *gameMode = GameModes::getGameMode(json.value("game_mode").toString());
#else
	IGameMode *gameMode = nullptr;
#endif
	//var resources:Vector.<String> = new Vector.<String>();
	//for each (s in json.resources)
	//resources.push(s);



	const int idleKickPeriodMsec = json.value("kick_period_ms").toInt();
	const QString mapDescriptorResourceId = json.value("map_id").toString();
	const int respawnInvulnerabilityPeriodMsec = json.value("invisible_time").toInt();
	const QString skyboxTextureResourceId = json.value("skybox_id").toString();
	bool spectator = json.value("spectator").toBool();

	//void BattlefieldModel::initObject(ClientObject *clientObject, BattlefieldResources *battlefieldResources, BattlefieldSoundScheme *battlefieldSoundScheme, int idleKickPeriodMsec, const QString &mapDescriptorResourceId, int respawnInvulnerabilityPeriodMsec, const QString &skyboxTextureResourceId, bool spectator, IGameMode *gameMode)
	this->battle->initObject(client,
							 nullptr,
							 nullptr,
							 idleKickPeriodMsec,
							 mapDescriptorResourceId,
							 respawnInvulnerabilityPeriodMsec,
							 skyboxTextureResourceId,
							 spectator,
							 gameMode);
	this->battle_init = true;

	Service::getIMain()->showBattle();
}


void BattleController::initLocalClientObject(const QString &id, const QString &name)
{
	_function_name("BattleController::initLocalClientObject");

	//var clientClass:ClientClass = new ClientClass(id, null, name);
	ClientClass *clientClass = nullptr;
	ClientObject *clientObject = new ClientObject(id, clientClass, name, nullptr);
	client = clientObject;
}


void BattleController::parseSpawnCommand(const QByteArray &js)
{
	_function_name("BattleController::parseSpawnCommand");

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();

	Vector3 pos;
	readVector3(json, pos);

	Vector3 rot(0., 0., json.value("rot").toDouble());
	TankSpecification tankSpec;
	tankSpec.speed = json.value("speed").toDouble();
	tankSpec.turnSpeed = json.value("turn_speed").toDouble();
	tankSpec.turretRotationSpeed = json.value("turret_rotation_speed").toDouble();
	TankModel *tank_model = Service::getTankModel();

	tank_model->spawn(tanks->value(json.value("tank_id").toString()),
					  &tankSpec,
					  BattleTeamType_getType(json.value("team_type").toString()),
					  &pos,
					  &rot,
					  json.value("health").toInt(),
					  json.value("incration_id").toInt());
}



IWeaponController* BattleController::getWeaponController(ClientObjectWeapon *obj)
{
	_function_name("BattleController::getWeaponController");

	IWeaponController *weaponController = nullptr;

	const QString &id = *obj->weapon_name;

	if (id == "smoky") {
		if (smokyModel == nullptr)
			smokyModel = new SmokyModel();

		smokyModel->objectLoaded(obj);
		weaponController = smokyModel;

	} else if (id == "flamethrower") {
		if (flamethrowerModel == nullptr)
			flamethrowerModel = new FlamethrowerModel();

		QVariantMap s = WeaponsManager::getSpecialEntity(obj->getId());
		double cone_angle = s.value("cone_angle").toDouble();
		int cooling_speed = s.value("cooling_speed").toInt();
		int heat_limit = s.value("heat_limit").toInt();
		int heating_speed = s.value("heating_speed").toInt();
		double range = s.value("range").toDouble();
		int target_detection_interval = s.value("target_detection_interval").toInt();
		flamethrowerModel->initObject(obj,
									  cone_angle,
									  cooling_speed,
									  heat_limit,
									  heating_speed,
									  range,
									  target_detection_interval);
		weaponController = flamethrowerModel;
	} else if (id == "twins") {
		if (twinsModel == nullptr)
			twinsModel = new PlasmaModel();

		QString obj_id = obj->getId();
		QVariantMap s = WeaponsManager::getSpecialEntity(obj_id);
		double shot_radius = s.value("shot_radius").toDouble();
		double shot_range = s.value("shot_range").toDouble();
		double shot_speed = s.value("shot_speed").toDouble();

		twinsModel->initObject(WeaponsManager::getObjectFor(obj_id), shot_radius, shot_range, shot_speed);

		weaponController = twinsModel;

	} else if (id == "thunder") {
		if (thunderModel == nullptr)
			thunderModel = new ThunderModel();

		QString obj_id = obj->getId();
		QVariantMap s = WeaponsManager::getSpecialEntity(obj_id);

		double impactForce = s.value("impactForce").toDouble();
		double maxSplashDamageRadius = s.value("maxSplashDamageRadius").toDouble();
		double minSplashDamagePercent = s.value("minSplashDamagePercent").toDouble();
		double minSplashDamageRadius = s.value("minSplashDamageRadius").toDouble();

		thunderModel->initObject(WeaponsManager::getObjectFor(obj_id),
								 impactForce,
								 maxSplashDamageRadius,
								 minSplashDamagePercent,
								 minSplashDamageRadius);

		weaponController = thunderModel;

	} else if (id == "isida") {
		if (isidaModel == nullptr)
			isidaModel = new HealingGunModel();

		QString obj_id = obj->getId();

		QVariantMap s = WeaponsManager::getSpecialEntity(obj_id);
		double angle = s.value("angle").toDouble();
		int capacity = s.value("capacity").toInt();
		int chargeRate = s.value("chargeRate").toInt();
		int tickPeriod = s.value("tickPeriod").toInt();
		double coneAngle = s.value("coneAngle").toDouble();
		int dischargeRate = s.value("dischargeRate").toInt();
		double radius = s.value("radius").toDouble();

		isidaModel->initObject(WeaponsManager::getObjectFor(obj_id), angle, capacity, chargeRate, tickPeriod, coneAngle, dischargeRate, radius);

		weaponController = isidaModel;
	} else if (id == "shaft") {
		if (shaftModel == nullptr)
			shaftModel = new ShaftModel();

		QString obj_id = obj->getId();

		QVariantMap s = WeaponsManager::getSpecialEntity(obj_id);
		double max_energy = s.value("max_energy").toDouble();
		double charge_rate = s.value("charge_rate").toDouble();
		double discharge_rate = s.value("discharge_rate").toDouble();
		double elevation_angle_up = s.value("elevation_angle_up").toDouble();
		double elevation_angle_down = s.value("elevation_angle_down").toDouble();
		double vertical_targeting_speed = s.value("vertical_targeting_speed").toDouble();
		double horizontal_targeting_speed = s.value("horizontal_targeting_speed").toDouble();
		double inital_fov = s.value("inital_fov").toDouble();
		double minimum_fov = s.value("minimum_fov").toDouble();
		double shrubs_hiding_radius_min = s.value("shrubs_hiding_radius_min").toDouble();
		double shrubs_hiding_radius_max = s.value("shrubs_hiding_radius_max").toDouble();
		double impact_quick_shot = s.value("impact_quick_shot").toDouble();

		shaftModel->initObject(WeaponsManager::getObjectFor(obj_id),
							   max_energy,
							   charge_rate,
							   discharge_rate,
							   elevation_angle_up,
							   elevation_angle_down,
							   vertical_targeting_speed,
							   horizontal_targeting_speed,
							   inital_fov,
							   minimum_fov,
							   shrubs_hiding_radius_min,
							   shrubs_hiding_radius_max,
							   impact_quick_shot);

		weaponController = shaftModel;

	} else if (id == "frezee") {
		if (frezeeModel == nullptr)
			frezeeModel = new FreezeModel();

		QString obj_id = obj->getId();

		QVariantMap s = WeaponsManager::getSpecialEntity(obj->getId());
		double damageAreaConeAngle = s.value("damageAreaConeAngle").toDouble();
		double damageAreaRange = s.value("damageAreaRange").toDouble();
		int energyCapacity = s.value("energyCapacity").toInt();
		int energyDischargeSpeed = s.value("energyDischargeSpeed").toInt();
		int energyRechargeSpeed = s.value("energyRechargeSpeed").toInt();
		int weaponTickMsec = s.value("weaponTickMsec").toInt();

		frezeeModel->initObject(WeaponsManager::getObjectFor(obj_id),
								damageAreaConeAngle,
								damageAreaRange,
								energyCapacity,
								energyDischargeSpeed,
								energyRechargeSpeed,
								weaponTickMsec);

		weaponController = frezeeModel;

	} else if (id == "railgun") {
		if (railgunModel == nullptr)
			railgunModel = new RailgunModel();

		railgunModel->initObject(obj, 1200, 0.8);
		railgunModel->objectLoaded(obj);
		weaponController = railgunModel;
	} else if (id == "terminator") {
		if (terminatorModel == nullptr)
			terminatorModel = new TerminatorModel();

		terminatorModel->initObject(obj, 1200, 0.8);
		terminatorModel->objectLoaded(obj);
		weaponController = terminatorModel;

	} else if (id == "ricochet") {
		if (ricochetModel == nullptr)
			ricochetModel = new RicochetModel();

		QString obj_id = obj->getId();

		QVariantMap s = WeaponsManager::getSpecialEntity(obj->getId());

		int energyCapacity = s.value("energyCapacity").toInt();
		int energyPerShot = s.value("energyPerShot").toInt();
		double energyRechargeSpeed = s.value("energyRechargeSpeed").toDouble();
		double shotDistance = s.value("shotDistance").toDouble();
		double shotRadius = s.value("shotRadius").toDouble();
		double shotSpeed = s.value("shotSpeed").toDouble();

		ricochetModel->initObject(WeaponsManager::getObjectFor(obj_id),
								  energyCapacity,
								  energyPerShot,
								  energyRechargeSpeed,
								  shotDistance,
								  shotRadius,
								  shotSpeed);
		weaponController = ricochetModel;

	} else {
		if (weaponModelEmpty == nullptr)
			weaponModelEmpty = new WeaponModelEmpty();
		weaponController = weaponModelEmpty;
	}



	return weaponController;
}


void BattleController::parseShotsData(const QByteArray &js)
{
	_function_name("BattleController::parseShotsData");

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	QJsonArray weapons = json.value("weapons").toArray();
	for (int i = 0 ; i < weapons.count(); i++)
	{
		QJsonObject obj = weapons.at(i).toObject();
		QString id = obj.value("id").toString();
		ShotData *shotData = new ShotData(obj.value("reload").toInt());
		shotData->autoAimingAngleDown = obj.value("auto_aiming_down").toDouble();
		shotData->autoAimingAngleUp = obj.value("auto_aiming_up").toDouble();
		shotData->numRaysDown = obj.value("num_rays_down").toInt();
		shotData->numRaysUp = obj.value("num_rays_up").toInt();
		if (obj.value("has_wwd").toBool())
		{
			double max_damage_radius = obj.value("max_damage_radius").toDouble();
			double minimumDamagePercent = obj.value("minimumDamagePercent").toDouble();
			double minimumDamageRadius = obj.value("minimumDamageRadius").toDouble();
			WeaponWeakeningModel::initObject(WeaponsManager::getObjectFor(id), max_damage_radius, minimumDamagePercent, minimumDamageRadius);
		}

		WeaponsManager::shotDataInsert(id, shotData);
		WeaponsManager::setSpecialEntity(id, obj.value("special_entity").toObject().toVariantMap());
	}
}

void BattleController::moveTank(const QByteArray &js)
{
	_function_name("BattleController::moveTank");

	//battle;move;{"orientation":{"y":-0.0002840611683985689,"z":-0.5079813920792567,"x":0.03625192557686257},"specificationID":0,"angularVelocity":{"y":0.004471662428558073,"z":1.2421221178974964,"x":0.001936500815064522},"linearVelocity":{"y":611.2252960738007,"z":0.23896913149093932,"x":516.1430397084655},"control":65,"position":{"y":49.72967460835325,"z":389.61022631986225,"x":2146.3693408554836},"physTime":1455547,"tankId":""}
	//battle;move;{"orientation":{"y":-0.0008431395468244049,"z":0.19427336211702162,"x":0.03524587122057496},"specificationID":0,"physTime":696779,"control":73,"linearVelocity":{"y":755.0275379764233,"z":0.42311080413953306,"x":-243.31793374600818},"position":{"y":-3766.691234718764,"z":89.55373593045694,"x":3178.702464243173},"angularVelocity":{"y":0.005780378090927972,"z":-0.1743853100503691,"x":0.002077442405497938},"tankId":""}


	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject o = document.object();
#ifndef L_PRO
	QString tank_id = o.value("tank_id").toString();
#else
	QString tank_id = o.value("tankId").toString();
#endif
	ClientObjectUser *user = tanks->value(tank_id);
	if (user == nullptr)
		return;

	Vector3 pos, orient, line, angle;

	readVector3(o.value("position").toObject(), pos);

#ifndef L_PRO
	readVector3(o.value("orient").toObject(), orient);
	readVector3(o.value("line").toObject(), line);
	readVector3(o.value("angle").toObject(), angle);
	double turrDir = o.value("turretDir").toDouble();
	int ctrlBits = o.value("ctrlBits").toInt();
#else
	readVector3(o.value("orientation").toObject(), orient);
	readVector3(o.value("linearVelocity").toObject(), line);
	readVector3(o.value("angularVelocity").toObject(), angle);
	double turrDir = 0;
	int ctrlBits = o.value("control").toInt();
#endif
	Service::getTankModel()->move(user, &pos, &orient, &line, &angle, turrDir, ctrlBits, true);
}

void BattleController::removeUser(const QString &id)
{
	_function_name("BattleController::removeUser");

#ifdef L_GT
	//botModel->removeBot(id);
#endif
	ClientObjectUser *user = tanks->value(id);
	if (user == nullptr)
		return;
	tanks->remove(id);
	Service::getTankModel()->objectUnloaded(user);
}


void BattleController::parseFinishBattle(const QByteArray &json)
{
	_function_name("BattleController::parseFinishBattle");

	(void)json;
	/*
	var obj:Object;
	var stat:UserStat;
	var parser:Object = JSON.parse(json);
	var users:Array = new Array();
	for each (obj in parser.users)
	{
		stat = new UserStat(obj.kills, obj.deaths, obj.id, obj.rank, obj.score, obj.prize, BattleTeamType.getType(obj.team_type), obj.id);
		users.push(stat);
	}
	StatisticsModel(Main.osgi.getService(IBattlefieldGUI)).finish(null, users, (parser.time_to_restart / 1000));
	*/
	battle->battleFinish();
}




void BattleController::parseFire(ClientObjectUser *user, const QByteArray &js)
{
	_function_name("BattleController::parseFire");

	if (user == nullptr)
		return;


	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	TankData *tankData = tankModel->getTankData(user);
	if (tankData == nullptr)
		return;

	ClientObjectWeapon *turret = tankData->turret;
	if (turret == nullptr)
		return;

	const QString &id = *tankData->turret->weapon_name;

	if (id == "smoky") {
		if (smokyModel != nullptr) {
			QJsonObject hitPos = json.value("hitPos").toObject();
			Vector3 hitPoint;
			hitPoint.x = hitPos.value("x").toDouble();
			hitPoint.y = hitPos.value("y").toDouble();
			hitPoint.z = hitPos.value("z").toDouble();
			smokyModel->fire(user, hitPoint, json.value("victimId").toString(), 1);
		}
	} else if (id == "twins") {
		if (twinsModel != nullptr) {
			int shotId = json.value("shotId").toInt();
			QJsonObject hitPos = json.value("hitPos").toObject();
			Vector3 hitPoint;
			hitPoint.x = hitPos.value("x").toDouble();
			hitPoint.y = hitPos.value("y").toDouble();
			hitPoint.z = hitPos.value("z").toDouble();
			twinsModel->hit(user, user->getId(), shotId, &hitPoint, json.value("victimId").toString(), 0.3);
		}
	} else if (id == "thunder") {
		if (thunderModel != nullptr) {
			Vector3 _hitPos;
			Vector3 *hitPos = nullptr;
			QJsonObject json_hitPos = json.value("hitPos").toObject();
			QString mainTargetId = json.value("mainTargetId").toString();
			QJsonArray json_splashTargetIds = json.value("splashTargetIds").toArray();
			if (!json_hitPos.isEmpty()) {
				_hitPos.x = json_hitPos.value("x").toDouble();
				_hitPos.y = json_hitPos.value("y").toDouble();
				_hitPos.z = json_hitPos.value("z").toDouble();
				hitPos = &_hitPos;
			}
			int ids_length = json_splashTargetIds.size();
			QVector<QString> splashTargetIds;
			splashTargetIds.reserve(ids_length);
			for (int i = 0; i < ids_length; i++)
				splashTargetIds.append(json_splashTargetIds.at(i).toString());
			thunderModel->fire(user, user->getId(), hitPos, mainTargetId, 1, splashTargetIds);
		}
	} else if (id == "shaft") {
		if (shaftModel != nullptr) {
			Vector3 static_point;
			Vector3 *pstatic_point = nullptr;
			QJsonObject static_shot = json.value("static_shot").toObject();
			if (!static_shot.isEmpty()) {
				readVector3(static_shot, static_point);
				pstatic_point = &static_point;
			}
			QJsonArray targets = json.value("targets").toArray();
			QVector<ServerShaftTargetData*> ids;
			for (int i = 0; i < targets.count(); i++)
			{
				QJsonObject objj = targets.at(i).toObject().value("target").toObject();
				QJsonObject json_pos = objj.value("pos").toObject();
				QJsonObject json_point_pos = objj.value("point_pos").toObject();
				Vector3 hitPoint;
				Vector3 globalHitPoint;
				QString targetId = objj.value("id").toString();
				readVector3(json_pos, hitPoint);
				readVector3(json_point_pos, globalHitPoint);
				ids.append(new ServerShaftTargetData(&hitPoint, &globalHitPoint, targetId));
			}
			shaftModel->fire(user, pstatic_point, &ids);
			qDeleteAll(ids);
		}

	} else if (id == "railgun") {
		if (railgunModel != nullptr) {
			//QJsonArray json_targetInc = json.value("targetInc").toArray();
			QJsonArray json_hitPoints = json.value("hitPoints").toArray();
			QJsonArray json_targets = json.value("targets").toArray();
			//QJsonArray json_targetPostitions = json.value("targetPostitions").toArray();
			QVector<Vector3*> hitPoints;
			for (int i = 0; i < json_hitPoints.count(); i++) {
				Vector3 *v = new Vector3;
				readVector3(json_hitPoints.at(i).toObject(), *v);
				hitPoints.append(v);
			}
			QStringList targets;
			for (int i = 0; i < json_targets.count(); i++)
				targets.append(json_targets.at(i).toString());
			railgunModel->fire(user, hitPoints, targets);
			qDeleteAll(hitPoints);
		}
	} else if (id == "terminator") {
		if  (terminatorModel != nullptr) {
			QJsonArray json_hitPoints = json.value("hitPoints").toArray();
			QJsonArray json_targets = json.value("targets").toArray();
			bool doubleShot = json.value("double").toBool();
			QVector<Vector3*> hitPoints;
			for (int i = 0; i < json_hitPoints.count(); i++) {
				Vector3 *v = new Vector3;
				readVector3(json_hitPoints.at(i).toObject(), *v);
				hitPoints.append(v);
			}
			QStringList targets;
			for (int i = 0; i < json_targets.count(); i++)
				targets.append(json_targets.at(i).toString());
			terminatorModel->fire(user, hitPoints, targets, doubleShot);
			qDeleteAll(hitPoints);
		}
	}
}

void BattleController::parseChangeSpecTank(ClientObjectUser *client, const QByteArray &json)
{
	_function_name("BattleController::parseChangeSpecTank");

	if (client == nullptr)
		return;

	QJsonDocument document = QJsonDocument::fromJson(json);
	QJsonObject o = document.object();

	TankSpecification tankSpec;
	tankSpec.speed = o.value("speed").toDouble();
	tankSpec.turnSpeed =o.value("turnSpeed").toDouble();
	tankSpec.turretRotationSpeed = o.value("turretRotationSpeed").toDouble();
	tankModel->changeSpecification(client, tankSpec,o.value("immediate").toBool());
}



void BattleController::createDamageEffect(ClientObjectUser *user, int damage, uint32_t color)
{
	_function_name("BattleController::createDamageEffect");

	if (user == nullptr)
		return;

	TankData *tankData = user->tankData;
	if (tankData == nullptr || tankData->tank == nullptr)
		return;

	tankModel->createDamageEffect(tankData->tank, damage, color);
}

void BattleController::parseCTF(const QByteArray &js)
{
	_function_name("BattleController::parseCTF");

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	ctfModel = new CTFModel(this);
	Service::setCTFModel(ctfModel);

	Vector3 posBlueFlag;
	Vector3 posRedFlag;

	readVector3(json.value("posBlueFlag").toObject(), posBlueFlag);
	readVector3(json.value("posRedFlag").toObject(), posRedFlag);


	ctfModel->initObject(this->ctfObj, "flagBlueModel_pedestal",
						 "flagBlueModel_img", "flagBlue", "flagRedModel_pedestal", "flagRedModel_img", "flagRed",
						 nullptr, &posBlueFlag, &posRedFlag);

	FlagsState flagsState;
	ClientFlag *blueFlag = &flagsState.blueFlag;
	readVector3(json.value("basePosBlueFlag").toObject(), blueFlag->flagBasePosition);
	readVector3(json.value("posBlueFlag").toObject(), blueFlag->flagPosition);
	blueFlag->flagCarrierId = json.value("blueFlagCarrierId").toString();
	ClientFlag *redFlag = &flagsState.redFlag;
	readVector3(json.value("basePosRedFlag").toObject(), redFlag->flagBasePosition);
	readVector3(json.value("posRedFlag").toObject(), redFlag->flagPosition);
	redFlag->flagCarrierId = json.value("redFlagCarrierId").toString();
	ctfModel->initFlagsState(this->ctfObj, &flagsState);
}

void BattleController::flagDrop(const QByteArray &js)
{
	_function_name("BattleController::flagDrop");

	if (ctfModel == nullptr)
		return;

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	Vector3 pos;
	pos.x = json.value("x").toDouble();
	pos.y = json.value("y").toDouble();
	pos.z = json.value("z").toDouble();
	ctfModel->dropFlag(nullptr, &pos, BattleTeamType_getType(json.value("flagTeam").toString()));
}



void BattleController::returnFlag(const QString &tankId, const QString &team)
{
	_function_name("BattleController::returnFlag");

	if (ctfModel == nullptr)
		return;

	ctfModel->returnFlagToBase(nullptr, BattleTeamType_getType(team), tankId);
}

void BattleController::deliverFlag(const QString &tankId, const QString &team)
{
	_function_name("BattleController::deliverFlag");

	if (ctfModel == nullptr)
		return;

	ctfModel->flagDelivered(nullptr, BattleTeamType_getType(team), tankId);
}


void BattleController::parseHP(ClientObjectUser *user, double damage, int color_id)
{
	_function_name("BattleController::parseHP");

	//var td:TankData = TankModel(Main.osgi.getService(ITank)).getTankData((activeTanks[user.id] as ClientObject));

	uint32_t color;
	switch (color_id)
	{
		case 1:
			color = 0x000000;
			break;
		case 2:
			color = 0x008000;
			break;
		case 3:
			color = 0xFFFF00;
			break;
		default:
			color = 0xFF0000;
			break;
	}

	if (user == nullptr)
		return;
	TankData *tankData = user->tankData;
	if (tankData == nullptr)
		return;
	Service::getIMain()->createDamageEffect(tankData->tank, damage, color);
}

void BattleController::moveAnticheatTank(const QByteArray &js)
{
	_function_name("BattleController::moveAnticheatTank");

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	Vector3 position;
	Vector3 orient;
	Vector3 line;
	Vector3 angle;
	readVector3(json.value("position").toObject(), position);
	readVector3(json.value("orient").toObject(), orient);
	readVector3(json.value("line").toObject(), line);
	readVector3(json.value("angle").toObject(), angle);

	ClientObjectUser *user = tanks->value(json.value("tank_id").toString());
	if (user == nullptr)
		return;

	double turretDir = json.value("turretDir").toDouble();
	int ctrlBits = json.value("ctrlBits").toInt();
	tankModel->moveAnticheat(user, &position, &orient, &line, &angle, turretDir, ctrlBits, true);
}

void BattleController::onChatMessage(const QByteArray &js)
{
	_function_name("BattleController::onChatMessage");

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	BattleChatMessage message;

	QString text = json.value("message").toString();
	if (json.value("system").toBool()) {
		message.setSystem(text);
	} else {
		QString nickname = json.value("nickname").toString();
		int rank = json.value("rank").toInt();
		BattleTeamType team_type = BattleTeamType_getType(json.value("team_type").toString());
		bool team = json.value("team").toBool();
		message.setUser(text, nickname, rank, team, team_type);
	}
	Service::getIMain()->battleChatMessage(message);
}


void BattleController::onChatMessageSpectator(const QString &text)
{
	_function_name("BattleController::onChatMessageSpectator");

	BattleChatMessage message;
	message.setSpectator(text);
	Service::getIMain()->battleChatMessage(message);
}



void BattleController::moveBot(const QByteArray &js)
{
#ifdef L_GT
	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;


	ClientObjectUser *user = tanks->value(json.value("botName").toString());
	if (user == nullptr)
		return;

	Vector3 pos, orient, line, ange;
	readVector3(json.value("position").toObject(), pos);
	readVector3(json.value("orientation").toObject(), orient);
	readVector3(json.value("velocity").toObject(), line);
	readVector3(json.value("angleVelocity").toObject(), ange);
	double turretAngle = json.value("turretAngle").toDouble();
	int ctrl = json.value("ctrl").toInt();
	tankModel->move(user, &pos, &orient, &line, &ange, turretAngle, ctrl, true);
#else
	(void)js;
#endif
}


void BattleController::parseDOM(const QByteArray &js)
{
	_function_name("BattleController::parseDOM");

	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;


	domModel = new DOMModel();
	QVector<std::shared_ptr<DOMPointData>> points;

	QJsonArray json_points = json.value("points").toArray();
	for (int i = 0; i < json_points.count(); i++)
	{
		QJsonObject obj = json_points.at(i).toObject();

		std::shared_ptr<DOMPointData> point = std::make_shared<DOMPointData>();
		point->radius = obj.value("radius").toDouble();
		readVector3(obj, point->pos);

#ifdef L_GT
		point->id = DomPointID(obj.value("id").toString().toLocal8Bit());
		point->pos.z += 300;
#else
        point->id = DomPointID(obj.value("id").toInt());;
#endif

		point->score = obj.value("score").toDouble();

		//point->occupatedUsers = new Vector.<String>();
		QJsonArray occupated_users = obj.value("occupated_users").toArray();
		for (int i = 0; i < occupated_users.count(); i++)
		{
			QString userId = occupated_users.at(i).toString();
			point->occupatedUsers.append(userId);
		}

		points.append(point);
	}

	domModel->initObject(points);
	domModel->objectLoaded(nullptr);

	Service::setDOMModel(domModel);
}

void BattleController::parseSpawnBonus(const QByteArray &js)
{
	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	QString id = json.value("id").toString();

	QString objId = id.split("_")[0];
	ClientObjectBonus *bonus_obj = this->clientObjectBonuses->value(objId);

	if (bonus_obj == nullptr)
	{
		bonus_obj = new ClientObjectBonus(objId, objId);
		this->clientObjectBonuses->insert(objId, bonus_obj);
	}

	int disappearing_time = json.value("disappearing_time").toInt();

	Vector3 pos;
	readVector3(json, pos);
	battle->addBonus(bonus_obj, id, id, &pos, disappearing_time);
}


void BattleController::putMine(const QByteArray &js)
{
	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	QString mineId = json.value("mineId").toString();
	QString userId = json.value("userId").toString();
	double x = json.value("x").toDouble();
	double y = json.value("y").toDouble();
	double z = json.value("z").toDouble();

	ClientObjectUser *user = tanks->value(userId);
	if (user == nullptr)
		return;

	this->serverMinesModel->putMine(user, mineId, userId, x, y, z);
}


void BattleController::parseStartFire(ClientObjectUser *user, const QString &firing, const QByteArray &argsJSON)
{
	TankData *td = Service::getITank()->getTankData(user);

	if (td == nullptr || td->turret == nullptr || td->turret->getId().isEmpty())
	{
		return;
	}

	QString id = td->turret->getId().split("_")[0];

	if (id == "railgun") {
		if (railgunModel != nullptr)
			railgunModel->startFire(user, firing);
	} else if (id == "flamethrower") {
		if (flamethrowerModel != nullptr)
			flamethrowerModel->startFire(user, firing);
	} else if (id == "isida") {
		if (isidaModel != nullptr) {
			QJsonDocument document = QJsonDocument::fromJson(argsJSON);
			QJsonObject object = document.object();
			if (object.isEmpty())
				return;
			IsisAction action;
			action.shooterId = object.value("shooterId").toString();
			action.targetId = object.value("targetId").toString();
			action.type = IsisActionTypeFromString(object.value("type").toString());
			isidaModel->startWeapon(user, &action);
		}

	} else if (id == "frezee") {
		if (frezeeModel != nullptr)
			frezeeModel->startFire(user, firing);
	} else if (id == "ricochet") {
		if (ricochetModel != nullptr) {
			QJsonDocument document = QJsonDocument::fromJson(argsJSON);
			QJsonObject object = document.object();
			if (object.isEmpty())
				return;
			double x = object.value("x").toDouble();
			double y = object.value("y").toDouble();
			double z = object.value("z").toDouble();
			ricochetModel->fire(user, firing, x, y, z);
		}
	}

}

void BattleController::parseStopFire(ClientObjectUser *user, const QString &firing)
{
	TankData *td = Service::getITank()->getTankData(user);

	if (td == nullptr || td->turret == nullptr || td->turret->getId().isEmpty())
	{
		return;
	}

	QString id = td->turret->getId().split("_")[0];

	if (id == "flamethrower") {
		if (flamethrowerModel != nullptr)
			flamethrowerModel->stopFire(user, firing);
	} else if (id == "isida") {
		if (isidaModel != nullptr)
			isidaModel->stopWeapon(user, firing);
	} else if (id == "frezee") {
		if (frezeeModel != nullptr)
			frezeeModel->stopFire(user, firing);
	}

}

void BattleController::parseStartFireTwins(ClientObjectUser *user, const QByteArray &json)
{
	if (user == nullptr || twinsModel == nullptr)
		return;

	QJsonDocument document = QJsonDocument::fromJson(json);
	QJsonObject object = document.object();
	if (object.isEmpty())
		return;

	int currBarrel = object.value("currBarrel").toInt();
	int realShotId = object.value("realShotId").toInt();

	QJsonObject objectDirToTarget = object.value("dirToTarget").toObject();

	Vector3 dirToTarget;
	dirToTarget.x = objectDirToTarget.value("x").toDouble();
	dirToTarget.y = objectDirToTarget.value("y").toDouble();
	dirToTarget.z = objectDirToTarget.value("z").toDouble();

	twinsModel->fire(user, user->getId(), currBarrel, realShotId, &dirToTarget);
}


void BattleController::parseShaftQuickFire(const QString &userName, const QByteArray &json)
{
	ClientObjectUser *user = this->tanks->value(userName);
	if (user == nullptr || shaftModel == nullptr)
		return;

	QJsonDocument document = QJsonDocument::fromJson(json);
	QJsonObject object = document.object();
	if (object.isEmpty())
		return;


	Vector3 dir;
	readVector3(object.value("dir").toObject(), dir);

	QVector<Vector3*> hitPoints;
	QJsonArray json_hitPoints = object.value("hitPoints").toArray();
	for (int i = 0; i < json_hitPoints.count(); i++)
	{
		Vector3 *vector = new Vector3();
		readVector3(json_hitPoints.at(i).toObject(), *vector);
		hitPoints.append(vector);
	}


	QStringList targets;
	QJsonArray json_targets = object.value("targets").toArray();
	for (int i = 0; i < json_targets.count(); i++)
	{
		targets.append(json_targets.at(i).toObject().value("target_id").toString());
	}

	shaftModel->quickFire(user, targets, &hitPoints, &dir);
	qDeleteAll(hitPoints);
}



void BattleController::movementControl(const QByteArray &json)
{
	//battle;movementControl;{"physTime":173356,"specificationID":0,"control":32,"tankId":""}

	QJsonDocument document = QJsonDocument::fromJson(json);
	QJsonObject object = document.object();
	if (object.isEmpty())
		return;

	QString tankId = object.value("tankId").toString();
	int control = object.value("control").toInt();

	ClientObjectUser *user = tanks->value(tankId);
	if (user == nullptr)
		return;

	tankModel->movementControl(user, control);
}

void BattleController::rotateTurret(const QByteArray &json)
{
	//battle;rotateTurret;{"physTime":173323,"angle":1.8997031268129148,"incarnation":26,"control":32,"tankId":""}

	QJsonDocument document = QJsonDocument::fromJson(json);
	QJsonObject object = document.object();
	if (object.isEmpty())
		return;

	QString tankId = object.value("tankId").toString();

	ClientObjectUser *user = tanks->value(tankId);
	if (user == nullptr)
		return;

	RotateTurretCommand rotateTurretCommand(object.value("angle").toDouble(), object.value("control").toInt());
	tankModel->rotateTurret(user, &rotateTurretCommand);
}
