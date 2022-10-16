#ifndef DOMMODEL_H
#define DOMMODEL_H

#include <QVector>
#include <QHash>
#include "alternativa/tanks/models/battlefield/IBattlefieldPlugin.h"
#include "dompointid.h"
#include <memory>

class DOMPointData;
class ClientObjectUser;
class ClientObject;
class Vector3;
class DomPoint;
class Object3D;
class TankData;
class BattlefieldModel;
class ITank;
class Network;
class BattlefieldData;
class Tank;
class BeamEffects;
class AllBeamProperties;
class ControlPointMarkers;
class TankResource;


class DOMModel : public IBattlefieldPlugin
{
	QHash<DomPointID, DomPoint*> pointsById;
	Network *network;
	BattlefieldData *bfData;
	QHash<TankData*, Tank*> tanksInBattle;

	/*
	private var pointsById:Dictionary = new Dictionary();
	private var tanksInBattle:Dictionary = new Dictionary();
	private var tankModel:ITank;
	*/
	BeamEffects *beamEffects;

	AllBeamProperties *allBeamProperties;
	/*
	private var pointHuds:KeyPointMarkers;
	private var bfData:BattlefieldData;
	private var startCapturingSound:Sound;
	private var stopCapturingSound:Sound;
	private var pointCapturedSound:Sound;
	private var enemyPointCapturedSound:Sound;
	private var enemyLostPointSound:Sound;
	private var lostPointSound:Sound;
	private var guiModel:IBattlefieldGUI;
	private var pointHUD:Vector.<KeyPointsHUDPanel> = new Vector.<KeyPointsHUDPanel>();
	*/


	QVector<ControlPointMarkers*> pointMarkers;

	BattlefieldModel *battlefieldModel;
	ITank *tankModel;

	std::shared_ptr<Object3D> createPedestal(TankResource *tankResource, const char *resourceImage);

public:

	TankData *userTankData;
	QVector<DomPoint*> points;

	DOMModel();
	virtual ~DOMModel();

	void initObject(QVector<std::shared_ptr<DOMPointData>> &points);
	void tankCapturingPoint(DomPoint *point, TankData *tankData);
	void tankLeaveCapturingPoint(DomPoint *point, TankData *tank);
	void serverSetPointScore(DomPointID &pointId, int score);
	void serverPointCapturedBy(const QString &team, DomPointID &pointId);
	void serverPointLostBy(const QString &team, DomPointID &pointId);
	void updatePedestal(DomPointID &pointId, const QString &team);
	void serverTankCapturingPoint(DomPointID &pointId, ClientObjectUser *tank);
	void serverTankLeaveCapturingPoint(ClientObjectUser *tank, DomPointID &pointId);
	QString getPositionString(const Vector3 *vector);
	double isPositionInPointProximity(const Vector3 *position, double distanceSquared);
	void tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff);
	void objectLoaded(ClientObject *object);
	void objectUnloaded(ClientObject *object);
	QString getBattlefieldPluginName();
	void startBattle();
	void restartBattle();
	void finishBattle();
	void addUser(ClientObjectUser *clientObject);
	void removeUser(ClientObjectUser *clientObject);
	void addUserToField(ClientObjectUser *clientObject);
	void removeUserFromField(ClientObjectUser *clientObject);
};

#endif // DOMMODEL_H
