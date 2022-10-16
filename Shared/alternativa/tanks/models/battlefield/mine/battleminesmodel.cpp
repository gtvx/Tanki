#include "battleminesmodel.h"
#include "com/alternativaplatform/projects/tanks/client/models/battlefield/mine/battlemine.h"
#include "resource/resourceutil.h"
#include "resource/tanks/tankresource.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "service.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/models/tank/ITank.h"
#include "usermineslist.h"
#include "proximitymine.h"
#include "alternativa/tanks/models/weapon/WeaponConst.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/core/Sorting.h"
#include "alternativa/physics/collision/primitives/collisionsphere.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "alternativa/physics/contact.h"
#include "networking/network.h"
#include "_global.h"
#include <QString>
#include <QVector>
#include <QHash>

//private static var objectPoolService:IObjectPoolService = IObjectPoolService(Main.osgi.getService(IObjectPoolService));
//private static const MAIN_EXPLOSION_FRAME_WIDTH:int = 300;
//private static const IDLE_EXPLOSION_FRAME_WIDTH:int = 100;
//private static const DECAL_RADIUS:Number = 200;
//private static var mainExplosionScale:ConsoleVarFloat = new ConsoleVarFloat("mine_main_scale", 3, 0.1, 10);
//private static var idleExplosionScale:ConsoleVarFloat = new ConsoleVarFloat("mine_idle_scale", 3, 0.1, 10);
//private static var mainExplosionFPS:ConsoleVarFloat = new ConsoleVarFloat("mine_main_fps", 30, 1, 50);
//private static var idleExplosionFPS:ConsoleVarFloat = new ConsoleVarFloat("mine_idle_fps", 30, 1, 50);
//private static var materialRegistry:IMaterialRegistry;
//private static const DECAL:Class = BattleMinesModel_DECAL;



BattleMinesModel::BattleMinesModel()
{
	_function_name("BattleMinesModel::BattleMinesModel");
	//_interfaces.push(IModel, IBattleMinesModelBase, IObjectLoadListener);
	//MineExplosionCameraEffect.initVars();
	//IDumpService(Main.osgi.getService(IDumpService)).registerDumper(this);
	//this->explosionMarkMaterial = new TextureMaterial(new DECAL().bitmapData);
	deferredMines = nullptr;
	minesOnFieldById = new QHash<QString, ProximityMine*>;
	minesByUser = new QHash<QString, UserMinesList*>;
	contact = new Contact(0);
	battlefieldModel = nullptr;
}


BattleMinesModel::~BattleMinesModel()
{
	_function_name("BattleMinesModel::~BattleMinesModel");
	delete minesOnFieldById;
	delete minesByUser;
	delete contact;
}


void BattleMinesModel::initObject(ClientObject *clientObject,
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
								  const QString &redMineTextureId)
{
	_function_name("BattleMinesModel::initObject");

	(void)clientObject;
	(void)activationSoundId;
	(void)activationTimeMsec;
	(void)blueMineTextureId;
	(void)mainExplosionTextureId;
	(void)minDistanceFromBase;
	(void)modelId;
	(void)nearVisibilityRadius;
	(void)radius;
	(void)redMineTextureId;
	(void)explosionSoundId;
	(void)deactivationSoundId;
	(void)enemyMineTextureId;
	(void)friendlyMineTextureId;
	(void)idleExplosionTextureId;
	(void)impactForce;
	(void)farVisibilityRadius;


	this->battlefieldModel = nullptr;

	//materialRegistry = IMaterialRegistry(Main.osgi.getService(IMaterialRegistry));
	this->mineModelData.armingDuration = activationTimeMsec;
	this->mineModelData.armedFlashDuration = 100;
	this->mineModelData.armedFlashFadeDuration = 300;
	this->mineModelData.flashChannelOffset = 204;
	this->mineModelData.farRadius = farVisibilityRadius * 100;
	this->mineModelData.nearRadius = nearVisibilityRadius * 100;
	this->mineProximityRadius = radius * 100;
	this->impactForce = impactForce;
	this->minDistanceFromBaseSquared = (minDistanceFromBase * minDistanceFromBase) * 10000;
	//IResourceService *resourceService: = IResourceService(Main.osgi.getService(IResourceService));
	//this->mineArmedSound = this->getSound(activationSoundId, resourceService);
	//this->explosionSound = this->getSound(explosionSoundId, resourceService);
	//this->deactivationSound = this->getSound(deactivationSoundId, resourceService);

	TankResource *tankResource = ResourceUtil::getResourceModel(modelId);
	if (tankResource == nullptr)
		throw 14114;

	this->initReferenceMesh(tankResource->mesh);

	//var initParams:InitParams = new InitParams(blueMineTextureId, redMineTextureId, enemyMineTextureId, friendlyMineTextureId, idleExplosionTextureId, mainExplosionTextureId);
	//clientObject.putParams(BattleMinesModel, initParams);
}

void BattleMinesModel::initMines(ClientObject *clientObject, QVector<BattleMine*> &mines)
{
	_function_name("BattleMinesModel::initMines");

	(void)clientObject;
	if (!mines.isEmpty()) {
		this->deferredMines = new QVector<BattleMine*>;
		*this->deferredMines = mines;
	}
}

void BattleMinesModel::objectLoaded(ClientObject *clientObject)
{
	_function_name("BattleMinesModel::objectLoaded");

	(void)clientObject;

	//var modelService:IModelService = IModelService(Main.osgi.getService(IModelService));
	this->battlefieldModel = Service::getIBattleField();

	//this->ctfModel = CTFModel(Main.osgi.getService(ICaptureTheFlagModelBase));
	//this->domModel = DOMModel(Main.osgi.getService(IDOMModel));
	//this->inventoryModel = IInventory(modelService.getModelsByInterface(IInventory)[0]);
	this->tankModel = Service::getITank();

	this->bfData = this->battlefieldModel->getBattlefieldData();
	this->battlefieldModel->addPlugin(this);

	//var resourceService:IResourceService = IResourceService(Main.osgi.getService(IResourceService));
	//var initParams:InitParams = InitParams(clientObject.getParams(BattleMinesModel));

	//double mipMapResolution = 1;
	//this->mainExplosionSequence = this->getExplosionMaterialSequence(initParams.mainExplosionTextureId, MAIN_EXPLOSION_FRAME_WIDTH, mipMapResolution, resourceService, this->mainExplosionFrameSize);
	//this->idleExplosionSequence = this->getExplosionMaterialSequence(initParams.idleExplosionTextureId, IDLE_EXPLOSION_FRAME_WIDTH, mipMapResolution, resourceService, this->idleExplosionFrameSize);
	//this->redMineMaterial = this->getTextureMaterial(initParams.redMineTextureId, mipMapResolution, resourceService);
	//this->blueMineMaterial = this->getTextureMaterial(initParams.blueMineTextureId, mipMapResolution, resourceService);
	//this->friendlyMineMaterial = this->getTextureMaterial(initParams.friendlyMineTextureId, mipMapResolution, resourceService);
	//this->enemyMineMaterial = this->getTextureMaterial(initParams.enemyMineTextureId, mipMapResolution, resourceService);


	//BattlefieldModel *bfModel = BattlefieldModel(Main.osgi.getService(IBattleField));
	//if (bfModel.blacklist.indexOf(this->redMineMaterial.getTextureResource()) == -1)
		//bfModel.blacklist.push(this->redMineMaterial.getTextureResource());
	//if (bfModel.blacklist.indexOf(this->blueMineMaterial.getTextureResource()) == -1)
		//bfModel.blacklist.push(this->blueMineMaterial.getTextureResource());
	//if (bfModel.blacklist.indexOf(this->friendlyMineMaterial.getTextureResource()) == -1)
		//bfModel.blacklist.push(this->friendlyMineMaterial.getTextureResource());
	//if (bfModel.blacklist.indexOf(this->enemyMineMaterial.getTextureResource()) == -1)
		//bfModel.blacklist.push(this->enemyMineMaterial.getTextureResource());
}

void BattleMinesModel::objectUnloaded(ClientObject *clientObject)
{
	_function_name("BattleMinesModel::objectUnloaded");

	(void)clientObject;
	this->removeAllMines();
	if (this->battlefieldModel != nullptr)
	{
		this->battlefieldModel->removePlugin(this);
		this->battlefieldModel = nullptr;
	}
	this->ctfModel = nullptr;
	this->domModel = nullptr;
	this->tankModel = nullptr;
	this->inventoryModel = nullptr;
	this->bfData = nullptr;
	//this->mineArmedSound = nullptr;
	//this->mainExplosionSequence = nullptr;
	//this->idleExplosionSequence = nullptr;
	//this->redMineMaterial = nullptr;
	//this->blueMineMaterial = nullptr;
	//this->friendlyMineMaterial = nullptr;
	//this->enemyMineMaterial = nullptr;
}


void BattleMinesModel::putMine(ClientObjectUser *clientObject, const QString &id, double x, double y, double z, const QString &userId)
{
	_function_name("BattleMinesModel::putMine");

	(void)clientObject;
	(void)id;
	(void)x;
	(void)y;
	(void)z;
	(void)userId;

	TankData *ownerData = nullptr;

	ClientObjectUser *ownerObject = clientObject;
	if (ownerObject != nullptr)
		ownerData = this->tankModel->getTankData(ownerObject);

	if (((ownerData == nullptr) || (ownerData->tank == nullptr)))
	{
		if (this->deferredMines == nullptr)
			this->deferredMines = new QVector<BattleMine*>;

		BattleMine *mine = new BattleMine;
		mine->activated = false;
		mine->mineId = id;
		mine->ownerId = userId;
		mine->x = x;
		mine->y = y;
		mine->z = z;

		this->deferredMines->append(mine);
	}
	else
	{
		Vector3 pos(x, y, z);
		this->addMine(id, this->mineProximityRadius, &pos, userId, ownerData->teamType, this->getMineMaterial(ownerData), false);
	}

}

void BattleMinesModel::activateMine(ClientObject *clientObject, const QString &id)
{
	_function_name("BattleMinesModel::activateMine");

	(void)clientObject;

	ProximityMine *mine = this->minesOnFieldById->value(id);
	if (mine != nullptr)
	{
		mine->arm();
		//this->addSound3DEffect(this->mineArmedSound, mine.position, 0.3);
	}
	else
	{
		if (deferredMines != nullptr)
		{
			for (BattleMine *battleMine : *deferredMines)
			{
				if (battleMine->mineId == id)
				{
					battleMine->activated = true;
					return;
				}
			}
		}
	}
}

void BattleMinesModel::removeMines(ClientObject *clientObject, const QString &ownerId)
{
	_function_name("BattleMinesModel::removeMines");

	(void)clientObject;

	UserMinesList *userMines = this->minesByUser->value(ownerId);
	if (userMines == nullptr)
		return;

	ProximityMine *mine = userMines->head;
	while (mine != nullptr)
	{
		ProximityMine *currMine = mine;
		mine = mine->next;
		//this->addExplosionEffect(currMine->position, this->idleExplosionSequence.materials, this->idleExplosionFrameSize, idleExplosionScale, idleExplosionFPS, 0.5, 0.9);
		//this->addSound3DEffect(this->deactivationSound, currMine->position, 0.1);
		this->removeMine(currMine, userMines);
	}
}

void BattleMinesModel::explodeMine(ClientObjectUser *clientObject, const QString &id, const QString &userId)
{
	_function_name("BattleMinesModel::explodeMine");

	(void)clientObject;
	(void)userId;

	ProximityMine *mine = this->minesOnFieldById->value(id);
	if (mine == nullptr)
		return;

	UserMinesList *userMines = this->minesByUser->value(mine->ownerId);
	if (userMines == nullptr)
		return;

	//this->addExplosionEffect(mine.position, this->mainExplosionSequence.materials, this->mainExplosionFrameSize, mainExplosionScale.value, mainExplosionFPS.value, 0.5, 0.772);
	//this->addSound3DEffect(this->explosionSound, mine.position, 0.5);
	this->addExplosionDecal(mine);
	ClientObjectUser *userObject = clientObject;
	if (userObject != nullptr)
	{
		TankData *tankData = this->tankModel->getTankData(userObject);
		if (((!(tankData == nullptr)) && (!(tankData->tank == nullptr))))
		{
			tankData->tank->addWorldForceScaled(&mine->position, &mine->normal, WeaponConst::BASE_IMPACT_FORCE * this->impactForce);
		}
	}
	this->removeMine(mine, userMines);
}

void BattleMinesModel::addExplosionDecal(ProximityMine *mine)
{
	_function_name("BattleMinesModel::addExplosionDecal");

	this->projectionOrigin.copy(&mine->position);
	this->projectionOrigin.addScaled(100., &mine->normal);
	//this->battlefieldModel->addDecal(mine.position, this->projectionOrigin, DECAL_RADIUS, this->explosionMarkMaterial);
}

QString BattleMinesModel::getBattlefieldPluginName()
{
	_function_name("BattleMinesModel::getBattlefieldPluginName");
	return "mines";
}

void BattleMinesModel::startBattle()
{
	_function_name("BattleMinesModel::startBattle");
}

void BattleMinesModel::restartBattle()
{
	_function_name("BattleMinesModel::restartBattle");
}

void BattleMinesModel::finishBattle()
{
	_function_name("BattleMinesModel::finishBattle");
	this->removeAllMines();
}

void BattleMinesModel::tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff)
{
	_function_name("BattleMinesModel::tick");

	(void)deltaSec;
	(void)interpolationCoeff;

	TankData *localTankData = TankData::localTankData;

	QHashIterator<QString, UserMinesList*> i(*this->minesByUser);
	while (i.hasNext()) {
		i.next();
		UserMinesList *userMines = i.value();

		if (userMines != nullptr) {
			ProximityMine *mine = userMines->head;
			while (mine != nullptr)
			{
				ProximityMine *currMine = mine;
				mine = mine->next;
				if (localTankData != nullptr)
				{
					if ((((currMine->canExplode(localTankData)) && (!(currMine->hitCommandSent))) && (this->mineIntersects(currMine, localTankData, this->bfData->collisionDetector, this->contact))))
					{
						currMine->hitCommandSent = true;
						const Vector3 &v = localTankData->tank->state.pos;
						this->hitCommand(this->bfData->bfObject, currMine->id, v.x, v.y, v.z);
					}
				}
				currMine->update(time, deltaMsec, localTankData);
			}
		}
	}

	if ((((!(this->ctfModel == nullptr)) && (!(localTankData == nullptr))) && (localTankData->enabled)))
	{
		//const Vector3 &pos = localTankData->tank->state.pos;
		//bool inProximity = ((this->ctfModel.isPositionInFlagProximity(pos, this->minDistanceFromBaseSquared, BattleTeamType.BLUE)) || (this->ctfModel.isPositionInFlagProximity(pos, this->minDistanceFromBaseSquared, BattleTeamType.RED)));
		//this->inventoryModel.lockItem(InventoryItemType::MINE, InventoryLock.FORCED, inProximity);
	}

	if ((((!(this->domModel == nullptr)) && (!(localTankData == nullptr))) && (localTankData->enabled)))
	{
		//const Vector3 &pos = localTankData.tank.state.pos;
		//bool inProximity = this->domModel.isPositionInPointProximity(pos, this->minDistanceFromBaseSquared);
		//this->inventoryModel.lockItem(InventoryItemType.MINE, InventoryLock.FORCED, inProximity);
	}
}

void BattleMinesModel::hitCommand(ClientObject *bfObject, const QString &mineId, double x, double y, double z)
{
	_function_name("BattleMinesModel::hitCommand");

	(void)bfObject;
	(void)x;
	(void)y;
	(void)z;
	Service::getNetworker()->send("battle;mine_hit;" + mineId);
}

void BattleMinesModel::addUser(ClientObjectUser *clientObject)
{
	_function_name("BattleMinesModel::addUser");
	(void)clientObject;
}

void BattleMinesModel::removeUser(ClientObjectUser *clientObject)
{
	_function_name("BattleMinesModel::removeUser");
	(void)clientObject;
}

void BattleMinesModel::addUserToField(ClientObjectUser *clientObject)
{
	_function_name("BattleMinesModel::addUserToField");

	if (this->deferredMines == nullptr)
		return;

	TankData *tankData = this->tankModel->getTankData(clientObject);
	if (tankData->local)
	{
		for (int i = 0; i < this->deferredMines->length(); i++)
		{
			BattleMine *battleMine = this->deferredMines->value(i);
			ClientObjectUser *ownerObject = clientObject;
			if (ownerObject != nullptr)
			{
				TankData *ownerTankData = this->tankModel->getTankData(ownerObject);
				if (ownerTankData != nullptr && ownerTankData->tank != nullptr)
				{
					Vector3 pos(battleMine->x, battleMine->y, battleMine->z);
					this->addMine(battleMine->mineId, this->mineProximityRadius, &pos, battleMine->ownerId, ownerTankData->teamType, this->getMineMaterial(ownerTankData), battleMine->activated);

					this->deferredMines->removeAt(i);
					i--;
					delete battleMine;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < this->deferredMines->length(); i++)
		{
			BattleMine *battleMine = this->deferredMines->value(i);
			QString id = clientObject->getId();
			if (battleMine->ownerId == id)
			{
				Vector3 pos(battleMine->x, battleMine->y, battleMine->z);
				this->addMine(battleMine->mineId, this->mineProximityRadius, &pos, battleMine->ownerId, tankData->teamType, this->getMineMaterial(tankData), battleMine->activated);

				this->deferredMines->removeAt(i);
				i--;
				delete battleMine;
			}
		}
	}
}

void BattleMinesModel::removeUserFromField(ClientObjectUser *clientObject)
{
	_function_name("BattleMinesModel::removeUserFromField");

	(void)clientObject;
}

void BattleMinesModel::addExplosionEffect(Vector3 *position, void *materials, FrameSize *frameSize, double scale, double fps, double originX, double originY)
{
	_function_name("BattleMinesModel::addExplosionEffect");

	(void)position;
	(void)materials;
	(void)frameSize;
	(void)scale;
	(void)fps;
	(void)originX;
	(void)originY;

	//var explosion:AnimatedSpriteEffect = AnimatedSpriteEffect(objectPoolService.objectPool.getObject(AnimatedSpriteEffect));
	//explosion.init((scale * frameSize.width), (scale * frameSize.height), materials, position, 0, 50, fps, false, originX, originY);
	//this->battlefieldModel.addGraphicEffect(explosion);
}

void BattleMinesModel::addMine(const QString &mineId, double proximityRadius, Vector3 *position, const QString &ownerId, BattleTeamType teamType, Material *material, bool armed)
{
	_function_name("BattleMinesModel::addMine");

	RayIntersection intersection;

	if ((!(this->bfData->collisionDetector->intersectRayWithStatic(position, &Vector3::DOWN, CollisionGroup::STATIC, 10000000000., nullptr, &intersection))))
		return;

	UserMinesList *userMines = this->minesByUser->value(ownerId);
	if (userMines == nullptr)
	{
		userMines = new UserMinesList();
		this->minesByUser->insert(ownerId, userMines);
	}

	ProximityMine *mine = ProximityMine::create(mineId, ownerId, proximityRadius, this->referenceMesh, material, teamType, &this->mineModelData);
	userMines->addMine(mine);
	this->minesOnFieldById->insert(mineId, mine);
	mine->setPosition(&intersection.pos, &intersection.normal);
	mine->addToContainer(this->bfData->viewport->getMapContainer());
	if (armed)
		mine->arm();
}

void BattleMinesModel::removeAllMines()
{
	_function_name("BattleMinesModel::removeAllMines");

	if (this->minesByUser != nullptr)
	{
		QHashIterator<QString, UserMinesList*> i(*this->minesByUser);
		while (i.hasNext()) {
			i.next();
			UserMinesList *userMines = i.value();
			if (userMines != nullptr) {
				userMines->clearMines();
				delete userMines;
			}
		}
		this->minesByUser->clear();
	}

	{
		/*
		QHashIterator<QString, ProximityMine*> i(*this->minesOnFieldById);
		while (i.hasNext()) {
			i.next();
			ProximityMine *mine = i.value();
			delete mine;
		}
		*/
		this->minesOnFieldById->clear();
	}

	if (deferredMines != nullptr) {
		qDeleteAll(*this->deferredMines);
		delete deferredMines;
		this->deferredMines = nullptr;
	}
}

void BattleMinesModel::initReferenceMesh(std::shared_ptr<Mesh> &resourse)
{
	_function_name("BattleMinesModel::initReferenceMesh");

	this->referenceMesh = resourse;
	if (this->referenceMesh->sorting != Sorting::DYNAMIC_BSP)
	{
		this->referenceMesh->sorting = Sorting::DYNAMIC_BSP;
		this->referenceMesh->calculateFacesNormals(true);
		//this->referenceMesh->optimizeForDynamicBSP();
	}
}

bool BattleMinesModel::mineIntersects(ProximityMine *mine, TankData *tankData, ICollisionDetector *collisionDetector, Contact *contact)
{
	_function_name("BattleMinesModel::mineIntersects");

	CollisionBox *cb = tankData->tank->mainCollisionBox;
	CollisionSphere *mcp = (CollisionSphere*)mine->collisionPrimitive;
	return collisionDetector->getContact(cb, mcp, contact);
}

void BattleMinesModel::removeMine(ProximityMine *mine, UserMinesList *userMines)
{
	_function_name("BattleMinesModel::removeMine");

	this->minesOnFieldById->remove(mine->id);
	userMines->removeMine(mine);
}

MaterialSequence *BattleMinesModel::getExplosionMaterialSequence(const QString &resourceId, int frameWidth, double mipMapResolution, IResourceService *resourceService, FrameSize *frameSize)
{
	_function_name("BattleMinesModel::getExplosionMaterialSequence");

	(void)resourceId;
	(void)frameWidth;
	(void)mipMapResolution;
	(void)frameSize;
	(void)resourceService;

	/*
	var texture:BitmapData;
	var imageResource:ImageResource;
	if (resourceId != null)
	{
		imageResource = (ResourceUtil.getResource(ResourceType.IMAGE, resourceId) as ImageResource);
		if (imageResource != null)
		{
			texture = (imageResource.bitmapData as BitmapData);
		}
	}
	if (texture == null)
	{
		texture = new StubBitmapData(0xFF0000);
		frameWidth = texture.width;
	}
	frameSize.width = frameWidth;
	frameSize.height = texture.height;
	return (materialRegistry.materialSequenceRegistry.getSequence(MaterialType.EFFECT, texture, frameWidth, mipMapResolution));
	*/
	return nullptr;
}

TextureMaterial *BattleMinesModel::getTextureMaterial(const QString &resourceId, double mipMapResolution, IResourceService *resourceRegister)
{
	_function_name("BattleMinesModel::getTextureMaterial");

	(void)resourceId;
	(void)mipMapResolution;
	(void)resourceRegister;
	/*
	var texture:BitmapData;
	var imageResource:ImageResource;
	if (resourceId != null)
	{
		imageResource = (ResourceUtil.getResource(ResourceType.IMAGE, resourceId) as ImageResource);
		if (imageResource != null)
		{
			texture = (imageResource.bitmapData as BitmapData);
		}
	}
	if (texture == null)
	{
		texture = new StubBitmapData(0xFF0000);
	}
	return (materialRegistry.textureMaterialRegistry.getMaterial(MaterialType.EFFECT, texture, mipMapResolution, false));
	*/
	return nullptr;
}

Material *BattleMinesModel::getMineMaterial(TankData *ownerData)
{
	_function_name("BattleMinesModel::getMineMaterial");

	(void)ownerData;
	/*
	switch (ownerData.teamType)
	{
		case BattleTeamType.NONE:
			return ((ownerData == TankData.localTankData) ? this->friendlyMineMaterial : this->enemyMineMaterial);
		case BattleTeamType.BLUE:
			return (this->blueMineMaterial);
		case BattleTeamType.RED:
			return (this->redMineMaterial);
	}
	return (this->enemyMineMaterial);
	*/
	return nullptr;
}
