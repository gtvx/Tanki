#include "dommodel.h"
#include "service.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "dompoint.h"
#include "battlecontroller.h"
#include "server/DOMPointData.h"
#include "alternativa/engine3d/core/object3d.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/objects/bsp.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/ITank.h"
#include "double_to_string.h"
#include "networking/network.h"
#include "resource/tanks/tankresource.h"
#include "alternativa/object/clientobjectuser.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/resource/stubbitmapdata.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/models/dom/sfx/beameffects.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/dom/sfx/dominationbeameffect.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/dom/sfx/allbeamproperties.h"
#include "cp/controlpointmarkers.h"


DOMModel::DOMModel()
{
	//this->points = new Vector.<Point>();
	//objectPoolService = IObjectPoolService(Main.osgi.getService(IObjectPoolService));
	//var modelService:IModelService = IModelService(Main.osgi.getService(IModelService));
	this->userTankData = nullptr;
	this->battlefieldModel = Service::getBattlefieldModel();
	this->battlefieldModel->addPlugin(this);
#ifdef GRAPHICS
	this->allBeamProperties = new AllBeamProperties();
	this->beamEffects = new BeamEffects(this->battlefieldModel);
#endif
	this->bfData = this->battlefieldModel->getBattlefieldData();
	this->tankModel = Service::getITank();
	this->network = Service::getNetworker();






	//this->pointHuds = new KeyPointMarkers(this->battlefieldModel.bfData.viewport.camera, this->battlefieldModel);
}

DOMModel::~DOMModel()
{
	qDeleteAll(points);
	qDeleteAll(pointMarkers);
#ifdef GRAPHICS
	delete this->allBeamProperties;
	delete this->beamEffects;
#endif
}


void DOMModel::initObject(QVector<std::shared_ptr<DOMPointData>> &points)
{
	TankResource *tankResource = ResourceUtil::getResourceModelLocal("dom/pedestal.3ds");
	if (tankResource == nullptr)
		throw 14135;

	QVector<DomPoint*> _points;

	for (std::shared_ptr<DOMPointData> &data : points)
	{
		DomPoint *point = new DomPoint(data->id, &data->pos, data->radius, this);
		this->points.append(point);
		//this->pointHuds.addMarker(new KeyPointMarker(point));
		this->pointsById[point->id] = point;
		_points.append(point);

		this->serverSetPointScore(point->id, data->score);

		for (const QString &userId : data->occupatedUsers)
		{
			this->serverTankCapturingPoint(point->id, Service::getBattleController()->tanks->value(userId));
		}

		int progress = point->clientProgress;

		point->pedestal_red = this->createPedestal(tankResource, "dom/pedestalRC.jpg");
		point->pedestal_blue = this->createPedestal(tankResource, "dom/pedestalBC.jpg");
		point->pedestal_none = this->createPedestal(tankResource, "dom/pedestalNC.jpg");
		point->pedestal_none->position.copy(&point->pos);
		point->pedestal_none->position.z -= 300;
		point->pedestal_red->position.copy(&point->pedestal_none->position);
		point->pedestal_blue->position.copy(&point->pedestal_none->position);

		std::shared_ptr<Object3D> pedestal;

		if (progress <= -100) {
			pedestal = point->pedestal_red;
		} else if (progress >= 100) {
			pedestal = point->pedestal_blue;
		} else {
			pedestal = point->pedestal_none;
		}

		point->pedestal = pedestal;

		this->bfData->viewport->getMapContainer()->addChild(pedestal.get());
		ControlPointMarkers *pointMark = new ControlPointMarkers(this->battlefieldModel, &data->pos, point);
		this->pointMarkers.append(pointMark);
	}

	//this->guiModel.createPointsHUD(new KeyPointsHUDPanel(_loc1_)); //Sprite
}

std::shared_ptr<Object3D> DOMModel::createPedestal(TankResource *tankResource, const char *resourceImage)
{
	std::shared_ptr<BSP> pedestal = new_BSP();
	pedestal->createTree(tankResource->mesh);
#ifdef GRAPHICS
	std::shared_ptr<BitmapData> texture = ResourceUtil::getResourceImageLocal(resourceImage);
	if (texture == nullptr)
		texture = std::make_shared<StubBitmapData>(0xFFFF00);

	double resolution = pedestal->calculateResolution(texture->width(), texture->height());
	IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();
	TextureMaterial* material = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT, texture, resolution, false);
	pedestal->setMaterialToAllFaces(material);
#else
	(void)resourceImage;
#endif
	return pedestal;
}



void DOMModel::tankCapturingPoint(DomPoint *point, TankData *tankData)
{
#ifdef GRAPHICS
	ObjectPoolService *objectPoolService = battlefieldModel->getObjectPoolService();
	DominationBeamEffect *effect = objectPoolService->getDominationBeamEffect();

	Vector3 pointPos;
	point->readPos(&pointPos);
	effect->init(tankData->tank->skin->turretMesh, &pointPos, this->allBeamProperties->getBeamProperties(tankData->teamType), nullptr); //new Dictionary()
	this->beamEffects->addEffect(tankData->user, effect);
	//this->playSoundEffect(this->startCapturingSound, point);
#endif

	network->send("battle;tank_capturing_point;" + point->id.toString() + ";" + this->getPositionString(&tankData->tank->state.pos));
}

void DOMModel::tankLeaveCapturingPoint(DomPoint *point, TankData *tank)
{
	(void)tank;
#ifdef GRAPHICS
	this->beamEffects->removeEffect(tank->user);
#endif
	//this->playSoundEffect(this->stopCapturingSound, point);
	network->send("battle;tank_leave_capturing_point;" +  point->id.toString());
}

void DOMModel::serverSetPointScore(DomPointID &pointId, int score)
{
	DomPoint *point = this->pointsById.value(pointId);
	if (point != nullptr)
		point->clientProgress = score;
}

void DOMModel::serverPointCapturedBy(const QString &team, DomPointID &pointId)
{
	//this->guiModel.logUserAction(null, ((((team == "blue") ? "Синие" : "Красные") + " захватили точку ") + ((pointId == 0) ? "A" : ((pointId == 1) ? "B" : ((pointId == 2) ? "C" : ((pointId == 3) ? "D" : ((pointId == 4) ? "E" : ((pointId == 5) ? "F" : "G"))))))), null);
	//this->battlefieldModel.messages.addMessage(((team == "blue") ? 4691967 : 15741974), ((((team == "blue") ? "Синие" : "Красные") + " захватили точку ") + ((pointId == 0) ? "A" : ((pointId == 1) ? "B" : ((pointId == 2) ? "C" : ((pointId == 3) ? "D" : ((pointId == 4) ? "E" : ((pointId == 5) ? "F" : "G"))))))));
	this->updatePedestal(pointId, team);

	//if (this->userTankData == nullptr)
		//return;
	//DomPoint point = this->pointsById.value(pointId);
	//if (userTankData->teamType.getValue() == team.toUpperCase())
		//this->playSoundEffect(this->pointCapturedSound, point);
	//else
		//this->playSoundEffect(this->enemyPointCapturedSound, point);
}

void DOMModel::serverPointLostBy(const QString &team, DomPointID &pointId)
{

	(void)team;
	//this->guiModel.logUserAction(null, ((((team == "blue") ? "Синие" : "Красные") + " потеряли контроль над точкой ") + ((pointId == 0) ? "A" : ((pointId == 1) ? "B" : ((pointId == 2) ? "C" : ((pointId == 3) ? "D" : ((pointId == 4) ? "E" : ((pointId == 5) ? "F" : "G"))))))), null);
	//this->battlefieldModel.messages.addMessage(((team == "blue") ? 15741974 : 4691967), ((((team == "blue") ? "Синие" : "Красные") + " потеряли контроль над точкой ") + ((pointId == 0) ? "A" : ((pointId == 1) ? "B" : ((pointId == 2) ? "C" : ((pointId == 3) ? "D" : ((pointId == 4) ? "E" : ((pointId == 5) ? "F" : "G"))))))));
	this->updatePedestal(pointId, "none");
	//if (userTankData == nullptr)
		//return;
	//var point:Point = this->pointsById[pointId];
	//if (userTankData.teamType.getValue() == team.toUpperCase())
		//this->playSoundEffect(this->lostPointSound, point);
	//else
		//this->playSoundEffect(this->enemyLostPointSound, point);
}

void DOMModel::updatePedestal(DomPointID &pointId, const QString &team)
{
	DomPoint *point = this->pointsById.value(pointId);

	std::shared_ptr<Object3D> pedestal;

	if (team == "blue") {
		pedestal = point->pedestal_blue;
	} else if (team == "red") {
		pedestal = point->pedestal_red;
	} else {
		pedestal = point->pedestal_none;
	}

	if (point->pedestal != nullptr && point->pedestal != pedestal)
	{
		this->bfData->viewport->getMapContainer()->removeChild(point->pedestal.get());
		//pedestal->position.x = point->pos.x;
		//pedestal->position.y = point->pos.y;
		//pedestal->position.z = point->pos.z - 300;
		point->pedestal = pedestal;
		this->bfData->viewport->getMapContainer()->addChild(pedestal.get());
	}
}

void DOMModel::serverTankCapturingPoint(DomPointID &pointId, ClientObjectUser *tank)
{
	if (tank == nullptr)
		return;

#ifdef GRAPHICS
	DomPoint *point = this->pointsById.value(pointId);
	TankData *tankData = this->tankModel->getTankData(tank);
	if (tankData == nullptr || tankData->tank == nullptr)
		return;

	ObjectPoolService *objectPoolService = battlefieldModel->getObjectPoolService();
	DominationBeamEffect *effect = objectPoolService->getDominationBeamEffect();

	Vector3 pointPos;
	point->readPos(&pointPos);
	effect->init(tankData->tank->skin->turretMesh, &pointPos, this->allBeamProperties->getBeamProperties(tankData->teamType), nullptr); //new Dictionary()
	this->beamEffects->addEffect(tankData->user, effect);
#else
	(void)pointId;
	(void)tank;
#endif
	//this->playSoundEffect(this->startCapturingSound, point);
}

void DOMModel::serverTankLeaveCapturingPoint(ClientObjectUser *tank, DomPointID &pointId)
{
	(void)tank;
	(void)pointId;
	//Point *point = this->pointsById.value(pointId);
	//this->playSoundEffect(this->stopCapturingSound, point);
#ifdef GRAPHICS
	this->beamEffects->removeEffect(tank);
#endif
}

QString DOMModel::getPositionString(const Vector3 *vector)
{
	return double_to_string(vector->x) + ";" + double_to_string(vector->y) + ";" + double_to_string(vector->z);
}

double DOMModel::isPositionInPointProximity(const Vector3 *position, double distanceSquared)
{
	double minDist = 100000000.;
	for (DomPoint *point : this->points)
	{
		double distance = point->pos.distanceToSquared(position);
		if (minDist > distance) {
			minDist = distance;
		}
	}

	return minDist < distanceSquared;
}

void DOMModel::tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff)
{
	for (DomPoint *point : this->points)
	{
		point->tick(time, deltaMsec, deltaSec, interpolationCoeff);
	}

	//this->pointHuds.render(time, deltaMsec);

	for (ControlPointMarkers *marker : this->pointMarkers)
		marker->tick();

	//this->guiModel.updatePointsHUD();
}

void DOMModel::objectLoaded(ClientObject *object)
{
	(void)object;
	//for (DomPoint *point : this->points)
		//point.drawDebug(this->battlefieldModel.getBattlefieldData().viewport.getMapContainer());
	//this->pointHuds.show();
}

void DOMModel::objectUnloaded(ClientObject *object)
{
	(void)object;
	this->battlefieldModel->removePlugin(this);
	this->userTankData = nullptr;

	for (ControlPointMarkers *marker : this->pointMarkers)
		marker->destroy();
}

QString DOMModel::getBattlefieldPluginName()
{
	return "DOM";
}

void DOMModel::startBattle()
{
}

void DOMModel::restartBattle()
{
}

void DOMModel::finishBattle()
{
}

void DOMModel::addUser(ClientObjectUser *clientObject)
{
	(void)clientObject;
}

void DOMModel::removeUser(ClientObjectUser *clientObject)
{
	(void)clientObject;
}

void DOMModel::addUserToField(ClientObjectUser *clientObject)
{
	TankData *tankData = this->tankModel->getTankData(clientObject);
	if (this->bfData->localUser == clientObject)
		userTankData = tankData;

	this->tanksInBattle[tankData] = tankData->tank;
}

void DOMModel::removeUserFromField(ClientObjectUser *clientObject)
{
	TankData *tankData = this->tankModel->getTankData(clientObject);
	this->tanksInBattle.remove(tankData);
}
