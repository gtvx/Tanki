#include "botmodel.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/tankparts.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/TankSpecification.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/ClientTank.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "service.h"
#include "battlecontroller.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/battlefield/logic/updaters/remotehulltransformupdater.h"
#include <memory>


BotModel::BotModel()
{
	//this->botControllers = new Dictionary();
}


void BotModel::addBot(const QByteArray &js)
{
	QJsonDocument document = QJsonDocument::fromJson(js);
	QJsonObject json = document.object();

	//var posParser:Object;
	//var json:Object = JSON.parse(js);
	TankParts tankParts;
	tankParts.coloringObjectId = json.value("colormap_id").toString();
	tankParts.hullObjectId = json.value("hull_id").toString();
	tankParts.turretObjectId = json.value("turret_id").toString();



	int health = json.value("health").toInt();

	ClientTank *clientTank = new ClientTank();


	clientTank->tankSpecification.speed = json.value("speed").toDouble();
	clientTank->tankSpecification.turnSpeed = json.value("turn_speed").toDouble();
	clientTank->tankSpecification.turretRotationSpeed = json.value("turret_turn_speed").toDouble();



	TankState &state = clientTank->tankState;

	if (!json.value("state_null").toBool())
	{
		QStringList temp = json.value("position").toString().split(QChar('@'));
		state.health = health;
		state.orientation.x = 0;
		state.orientation.y = 0;
		state.orientation.z = temp[3].toDouble();
		state.position.x = temp[0].toDouble();
		state.position.y = temp[1].toDouble();
		state.position.z = temp[2].toDouble();
	} else {
		state.health = 0;
		state.orientation.reset();
		state.position.reset();
	}

	state.turretAngle = 0;
	state.control = 0;


	const QString nickname = json.value("nickname").toString();
	const QString tank_id = json.value("tank_id").toString();


	clientTank->health = health;
	clientTank->incarnationId = json.value("icration").toInt();
	clientTank->self = false;
	QString stateSpawn = json.value("state").toString();
	clientTank->spawnState = ((stateSpawn == "newcome") ? TankSpawnState::NEWCOME : ((stateSpawn == "active") ? TankSpawnState::ACTIVE : ((stateSpawn == "suicide") ? TankSpawnState::SUICIDE : TankSpawnState::ACTIVE)));
	clientTank->teamType = BattleTeamType_getType(json.value("team_type").toString());
	//ClientClass *clientClass = new ClientClass(json.tank_id, null, json.tank_id);
	std::shared_ptr<ClientObjectUser> clientObject = std::make_shared<ClientObjectUser>(tank_id, tank_id);

	BattleController *battleController = Service::getBattleController();
	battleController->tanks->insert(nickname, clientObject.get());

	TankModel *tankModelService = Service::getTankModel();
	//tankModelService->initObject(clientObject, json.battleId, json.mass, json.power, null, tankParts, null, json.impact_force, json.kickback, json.turret_rotation_accel, json.turret_turn_speed, json.nickname, json.rank, tankParts.turretObjectId);

	const QString battleId = json.value("battleId").toString();
	const double mass = json.value("mass").toDouble();
	const double power = json.value("power").toDouble();
	const double impactForce = json.value("impact_force").toDouble();
	const double kickback = json.value("kickback").toDouble();
	const double turretRotationAcceleration = json.value("turret_rotation_accel").toDouble();
	const double turretRotationSpeed = json.value("turret_turn_speed").toDouble();
	const int rank = json.value("rank").toInt();

	tankModelService->initObject(clientObject, battleId, mass, power, nullptr, &tankParts,
								 nullptr, impactForce, kickback,
								 turretRotationAcceleration, turretRotationSpeed,
								 nickname, rank, tankParts.turretObjectId);

	tankModelService->initTank(clientObject, clientTank, &tankParts, false);
	TankData *tankData = tankModelService->getTankData(clientObject.get());

	delete clientTank;

	/*
	Vector3 targetPos;
	QJsonObject current_waypoint = json.value("current_waypoint").toObject();
	if (!current_waypoint.isEmpty())
	{
		targetPos.x = current_waypoint.value("x").toDouble();
		targetPos.y = current_waypoint.value("y").toDouble();
		targetPos.z = current_waypoint.value("z").toDouble();
	}
	else
	{
		targetPos.reset();
	}
	*/


	if (tankData == nullptr || tankData->tank == nullptr)
		return;

	this->configureBot(clientObject.get(), nullptr);
}

void BotModel::removeBot(const QString &nickname)
{
	(void)nickname;
	/*
	TankModel *tankModelService = Service::getTankModel();
	TankData *tankData = tankModelService.getTankData(BattleController.activeTanks[nickname]);
	if (tankData != nullptr)
		this->botControllers[tankData.userName] = null;
		*/
}

void BotModel::destroy()
{
	//this.botControllers = new Dictionary();
}

bool BotModel::playerIsBot(const QString &nickname)
{
	(void)nickname;
	return false;
	//return (!(this.botControllers[nickname] == null));
}

bool BotModel::isBot(const QString &nickname)
{
	(void)nickname;
	return false;
	//return (!(this.botControllers[nickname] == null));
}

void BotModel::configureBot(ClientObjectUser *clientObject, Vector3 *targetPos)
{
	(void)targetPos;
	TankModel *tankModelService = Service::getTankModel();
	//TankModel *tankModelService = (Main.osgi.getService(ITank) as TankModel);
	TankData *tankData = tankModelService->getTankData(clientObject);
	tankData->tank->setHullTransformUpdater(new RemoteHullTransformUpdater(tankData->tank));
}
