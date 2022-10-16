#ifndef BATTLEMINESMODEL_H
#define BATTLEMINESMODEL_H

#include "alternativa/math/vector3.h"
#include "../IBattlefieldPlugin.h"
#include "MineModelData.h"
#include <qglobal.h>
#include <memory>

class IBattleField;
class CTFModel;
class DOMModel;
class IInventory;
class ITank;
class BattlefieldData;
class Mesh;
class FrameSize;
class ClientObject;
class Material;
class TextureMaterial;
class MaterialSequence;
class ProximityMine;
enum class BattleTeamType;
class IResourceService;
class ICollisionDetector;
class TankData;
class Contact;
class UserMinesList;
class Array;
class BattleMine;



class BattleMinesModel : public IBattlefieldPlugin
{
	IBattleField *battlefieldModel;
	CTFModel *ctfModel;
	DOMModel *domModel;
	IInventory *inventoryModel;
	ITank *tankModel;
	BattlefieldData *bfData;
	double mineProximityRadius;
	//QVector<BattleMine> redMineMaterial;
	//TextureMaterial *redMineMaterial;
	//TextureMaterial *blueMineMaterial;
	//TextureMaterial *friendlyMineMaterial;
	//TextureMaterial *enemyMineMaterial;
	std::shared_ptr<Mesh> referenceMesh;
	//MaterialSequence *mainExplosionSequence;
	//MaterialSequence *idleExplosionSequence;
	double impactForce;
	double minDistanceFromBaseSquared;
	//TextureMaterial *explosionMarkMaterial;

	MineModelData mineModelData;

	QHash<QString, UserMinesList*> *minesByUser;

	FrameSize *mainExplosionFrameSize;
	FrameSize *idleExplosionFrameSize;

	//RayIntersection *intersection;
	Contact *contact;
	Vector3 projectionOrigin;

	QVector<BattleMine*> *deferredMines;


public:

	QHash<QString, ProximityMine*> *minesOnFieldById;

	BattleMinesModel();
	virtual ~BattleMinesModel();

	void initObject(ClientObject *clientObject,
					const QString &activationSoundId,
					int activationTimeMsec,
					const QString &blueMineTextureId,
					const QString &deactivationSoundId,
					const QString &enemyMineTextureId,
					const QString &explosionSoundId,
					double farVisibilityRadius,
					const QString &friendlyMineTextureId,
					const QString &idleExplosionTextureId,
					double impactForce,
					const QString &mainExplosionTextureId,
					double minDistanceFromBase,
					const QString &modelId,
					double nearVisibilityRadius,
					double radius,
					const QString &redMineTextureId);

	void initMines(ClientObject *clientObject, QVector<BattleMine*> &mines);
	void objectLoaded(ClientObject *clientObject);
	void objectUnloaded(ClientObject *clientObject);
	void putMine(ClientObjectUser *clientObject, const QString &id, double x, double y, double z, const QString &userId);
	void activateMine(ClientObject *clientObject, const QString &id);
	void removeMines(ClientObject *clientObject, const QString &ownerId);
	void explodeMine(ClientObjectUser *clientObject, const QString &id, const QString &userId);
	void addExplosionDecal(ProximityMine *mine);
	QString getBattlefieldPluginName();
	void startBattle();
	void restartBattle();
	void finishBattle();
	void tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff);


private:
	void hitCommand(ClientObject *bfObject, const QString &mineId, double x, double y, double z);

public:
	void addUser(ClientObjectUser *clientObject);
	void removeUser(ClientObjectUser *clientObject);
	void addUserToField(ClientObjectUser *clientObject);
	void removeUserFromField(ClientObjectUser *clientObject);

private:
	void addExplosionEffect(Vector3 *position, void *materials, FrameSize *frameSize, double scale, double fps, double originX, double originY);
	void addMine(const QString &mineId, double proximityRadius, Vector3 *position, const QString &ownerId, BattleTeamType teamType, Material *material, bool armed);
	void removeAllMines();
	void initReferenceMesh(std::shared_ptr<Mesh> &resourse);
	bool mineIntersects(ProximityMine *mine, TankData *tankData, ICollisionDetector *collisionDetector, Contact *contact);
	void removeMine(ProximityMine *mine, UserMinesList *userMines);
	MaterialSequence* getExplosionMaterialSequence(const QString &resourceId, int frameWidth, double mipMapResolution, IResourceService *resourceService, FrameSize *frameSize);
	TextureMaterial* getTextureMaterial(const QString &resourceId, double mipMapResolution, IResourceService *resourceRegister);
	Material* getMineMaterial(TankData *ownerData);
};

#endif // BATTLEMINESMODEL_H
