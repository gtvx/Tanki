#include "serverbattleminesmodel.h"
#include "alternativa/tanks/models/battlefield/mine/battleminesmodel.h"
#include "alternativa/object/clientobject.h"
#include "com/alternativaplatform/projects/tanks/client/models/battlefield/mine/battlemine.h"
#include "alternativa/object/clientobjectuser.h"
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


static ClientObject __object("ServerBattleMinesModel_obj", nullptr, "ServerBattleMinesModel_obj", nullptr);


ServerBattleMinesModel::ServerBattleMinesModel(BattleMinesModel *model)
{
	this->model = model;
}

void ServerBattleMinesModel::init()
{

}

void ServerBattleMinesModel::initModel(const QByteArray &data)
{
	QJsonDocument document = QJsonDocument::fromJson(data);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	int activationTimeMsec = json.value("activationTimeMsec").toInt();
	double farVisibilityRadius = json.value("farVisibilityRadius").toDouble();
	double impactForce = json.value("impactForce").toDouble();
	double minDistanceFromBase = json.value("minDistanceFromBase").toDouble();
	double nearVisibilityRadius = json.value("nearVisibilityRadius").toDouble();
	double radius = json.value("radius").toDouble();

	this->model->initObject(&__object,
							"activationSound_mine",
							activationTimeMsec,
							"blueMineTexture",
							"deactivationSound_mine",
							"enemyMineTexture",
							"explosionSound_mine",
							farVisibilityRadius,
							"friendlyMineTexture_mine",
							"idleExplosionTexture_mine",
							impactForce,
							"mainExplosionTextureId_mine",
							minDistanceFromBase,
							"modelId_mine",
							nearVisibilityRadius,
							radius,
							"redMineTexture_mine");

	this->model->objectLoaded(&__object);
}

void ServerBattleMinesModel::initMines(const QByteArray &data)
{
	QJsonDocument document = QJsonDocument::fromJson(data);
	QJsonObject json = document.object();
	if (json.isEmpty())
		return;

	QVector<BattleMine*> mines;

	QJsonArray json_mines = json.value("mines").toArray();
	for (int i = 0; i < json_mines.count(); i++)
	{
		QJsonObject json_mine = json_mines.at(i).toObject();

		BattleMine *mine = new BattleMine;
		mine->activated = true;
		mine->mineId = json_mine.value("mineId").toString();
		mine->ownerId = json_mine.value("ownerId").toString();
		mine->x = json_mine.value("x").toDouble();
		mine->y = json_mine.value("y").toDouble();
		mine->z = json_mine.value("z").toDouble();

		mines.append(mine);
	}

	this->model->initMines(nullptr, mines);
}

void ServerBattleMinesModel::removeMines(const QString &ownerId)
{
	this->model->removeMines(nullptr, ownerId);
}

void ServerBattleMinesModel::putMine(ClientObjectUser *ownerObject, const QString &mineId, const QString &userId, double x, double y, double z)
{
	this->model->putMine(ownerObject, mineId, x, y, z, userId);
}

void ServerBattleMinesModel::activateMine(const QString &ownerId)
{
	this->model->activateMine(nullptr, ownerId);
}

void ServerBattleMinesModel::hitMine(ClientObjectUser *ownerObject, const QString &mineId)
{
	if (ownerObject == nullptr)
		return;
	this->model->explodeMine(ownerObject, mineId, ownerObject->getId());
}
