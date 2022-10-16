#include "shaftmodel.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "ShaftData.h"
#include "../WeaponConst.h"
#include <QVector>
#include "alternativa/object/clientobjectweaponshaft.h"
#include "ShaftEnergyMode.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/display/usertitle/usertitle.h"
#include "mymath.h"
#include "modes/targetingcontroller.h"
#include "states/ShaftModes.h"
#include "../AllGlobalGunParams.h"
#include "alternativa/tanks/vehicles/tanks/track.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "cameracontrollers/targetingcameracontroller.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "../common/IWeaponCommonModel.h"
#include "../common/weaponcommondata.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/models/weapon/weaponutils.h"
#include "quickshot/shafttargetsystem.h"
#include "quickshot/shaftshotresult.h"
#include "sfx/shaftsfxmodel.h"
#include "alternativa/tanks/vehicles/tanks/tankskinturret.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "battlecontroller.h"
#include "service.h"
#include "server/models/shaft/ServerShaftTargetData.h"
#include "aimedshotresult.h"
#include "aim.h"
#include "alternativa/engine3d/core/rayintersectiondata.h"
#include "alternativa/tanks/models/battlefield/object3dnames.h"
#include "shaftdeactivationtask.h"
#include "../shared/shot/shotdata.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/utils/mathutils.h"
#include "alternativa/physics/physicsscene.h"
#include "setcontrollerfortemporaryitems.h"
#include "alternativa/tanks/models/battlefield/objects2tank.h"
#include "alternativa/engine3d/core/object3dcontainer.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/models/battlefield/decals/RotationState.h"
#include "shaftindicator.h"
#include "flash/display/stage.h"
#include "flash/display/imageitem.h"
#include "networking/network.h"
#include "double_to_string.h"
#include "flash/display/bitmapdata.h"
#include "__global.h"

static const int thousand = 1000;

static void globalToLocal(Body *body, Vector3 *vector)
{
	vector->subtract(&body->state.pos);
	vector->transformBy3Tr(&body->baseMatrix);
}

static void localToGlobal(Body *body, Vector3 *out)
{
	out->transformBy3(&body->baseMatrix);
	out->add(&body->state.pos);
}

static void getBarrelOrigin(Vector3 *v, WeaponCommonData *weaponCommonData, int index = 0)
{
	v->copy(weaponCommonData->getMuzzle(index));
	v->y = 0;
}

static void getAllGunParams(Tank *tank, AllGlobalGunParams *gun, WeaponCommonData *weaponCommonData, int id = 0)
{
	WeaponUtils::calculateMainGunParams(tank->skin->turretMesh, weaponCommonData->getMuzzle(id), gun);
}

void ShaftModel::getShotDirection(Vector3 *param1, Vector3 *param2, Vector3 *param3, Vector3 *result)
{
	Vector3 const *v;

	if (param2 != nullptr)
		v = param2;
	else if (param3 != nullptr)
		v = param3;
	else
		v = &allGunParams->direction;

	result->diff(v, param1);

	result->normalize();
}

static void getClosestBarrelOrigin(Vector3 *param1, WeaponCommonData *commonData, Vector3 *param3)
{
	Vector3 _barrelOrigin;
	_barrelOrigin.copy(commonData->getMuzzle(0));
	_barrelOrigin.y = 0;
	param3->copy(&_barrelOrigin);
	double _loc4_ = param1->distanceToSquared(&_barrelOrigin);
	int i = 1;
	while (i < commonData->muzzleLength())
	{
		_barrelOrigin.copy(commonData->getMuzzle(i));
		_barrelOrigin.y = 0;
		double _loc6_ = param1->distanceToSquared(&_barrelOrigin);
		if (_loc6_ < _loc4_)
		{
			_loc4_ = _loc6_;
			param3->copy(&_barrelOrigin);
		}
		i++;
	}
}


ShaftModel::ShaftModel()
{
	this->energyMode = ShaftEnergyMode::RECHARGE;
	this->allGunParams = new AllGlobalGunParams();
	this->quickShotTargetSystem = new ShaftTargetSystem();
	this->shotResult = new ShaftShotResult();
	this->commonModel = nullptr;
	this->lock = false;
	this->updateController = false;
	this->energyAdditive = 1000;
	this->energySpeed = -250;
	this->activatedTime = 0;
	this->activate = false;
	this->quickShot = false;
	this->fired = false;
	this->exitTime = 0;
	this->energyBaseTime = 0;
	this->lockCheckIntersection = false;
	this->tank = nullptr;
	this->aim = false;
	this->image = nullptr;
	this->indicatorBitmap = nullptr;
	this->indicatorIndexFrame = 0;
	this->targetController = nullptr;

#ifdef GRAPHICS
	decalMaterial = new_TextureMaterial();
	decalMaterial->init();
	decalMaterial->setTexture(ResourceUtil::getResourceImageLocal("decal/shaft.png"));
#endif

}

ShaftModel::~ShaftModel()
{
#ifdef GRAPHICS
	if (this->image != nullptr)
		delete this->image;

	if (this->indicatorBitmap != nullptr)
		delete this->indicatorBitmap;
#endif

	if (this->targetController != nullptr)
		delete this->targetController;
}

void ShaftModel::initObject(ClientObjectWeapon *clientObject,
							double maxEnegry,
							double chargeRate,
							double dischargeRate,
							double elevationAngleUp,
							double elevationAngleDown,
							double verticalTargetingSpeed,
							double horizontalTargetingSpeed,
							double initialFOV,
							double minimumFOV,
							double shrubsHidingRadiusMin,
							double shrubsHidingRadiusMax,
							double impactQuickShot)
{
	std::shared_ptr<ShaftData> shaftData = std::make_shared<ShaftData>();
	shaftData->maxEnergy = maxEnegry;
	shaftData->chargeRate = chargeRate;
	shaftData->dischargeRate = dischargeRate;
	shaftData->elevationAngleUp = elevationAngleUp;
	shaftData->elevationAngleDown = elevationAngleDown;
	shaftData->verticalTargetingSpeed = verticalTargetingSpeed;
	shaftData->horizontalTargetingSpeed = horizontalTargetingSpeed;
	shaftData->initialFOV = initialFOV;
	shaftData->minimumFOV = minimumFOV;
	shaftData->shrubsHidingRadiusMin = shrubsHidingRadiusMin;
	shaftData->shrubsHidingRadiusMax = shrubsHidingRadiusMax;
	shaftData->impactQuickShot = impactQuickShot * WeaponConst::BASE_IMPACT_FORCE;
	ClientObjectWeaponShaft::setData(clientObject, shaftData);
	this->objectLoaded(nullptr);
}

void ShaftModel::objectLoaded(ClientObject *object)
{
	(void)object;

	//if (this->modelService == null)
	{
		//this->modelService = IModelService(Main.osgi.getService(IModelService));
		this->battlefieldModel = Service::getBattlefieldModel();
		this->tankModel = Service::getTankModel();
		this->weaponCommonModel = Service::getIWeaponCommonModel();
		//this->weaponWeakeningModel = IWeaponWeakeningModel(this->modelService.getModelsByInterface(IWeaponWeakeningModel)[0]);
		this->commonModel = Service::getIWeaponCommonModel();
		//(this->battlefieldModel as BattlefieldModel).blacklist.push(decalMaterial.getTextureResource());
		//this->exclusionSet = this->battlefieldModel.bfData.viewport.shaftRaycastExcludedObjects;
		this->exclusionSetController = new SetControllerForTemporaryItems(&this->exclusionSet);
		//this->collisionDetector = (TanksCollisionDetector*)this->battlefieldModel->bfData->physicsScene->collisionDetector;
		this->exclusionSetFilter.exclusionSet = &this->exclusionSet;
	}
}

void ShaftModel::setEnergyMode(ShaftEnergyMode mode)
{
	if (((this->battlefieldModel == nullptr) || (this->battlefieldModel->bfData == nullptr)))
	{
		return;
	}
	if (mode != this->energyMode)
	{
		int physTime = this->battlefieldModel->bfData->physTime;
		double energy = this->getEnergy(physTime);
		this->doSetEnergyMode(mode, energy, physTime);
	}
}

void ShaftModel::objectUnloaded(ClientObject *object)
{
	(void)object;
}

double ShaftModel::update(int param1, int param2)
{
#ifdef GRAPHICS
	if (aim == true)
	{
		UserTitle *title = tank->title();
		if (indicatorIndexFrame != title->indexFrame())
		{
			indicatorIndexFrame = title->indexFrame();
			if (indicatorBitmap != nullptr)
			{
				indicatorBitmap->upload();
			}
		}
	}
#endif

	TankData *tankData = TankData::localTankData;

	if (this->tank == nullptr)
	{
		this->tank = tankData->tank;
	}

	if (this->activate && tankData->spawnState == TankSpawnState::ACTIVE)
	{
		this->activatedTime = (this->activatedTime + param2);
	}
	else
	{
		if ((((tankData->spawnState == TankSpawnState::ACTIVE) && (this->activatedTime > 0)) && (this->activatedTime < 300)))
		{
			this->quickShot = true;
		}
		this->activatedTime = 0;
	}

	if (this->activatedTime >= 300)
	{
		int _loc3_ = this->getEnergy(param1);
		if (((!this->lock && _loc3_ == 1000) && (param1 >= this->exitTime)) &&
			 ((!((this->tank->leftTrack->lastContactsNum == 0) &&
				 (this->tank->rightTrack->lastContactsNum == 0))) || !this->hasIntersection()))
		{
			this->aim = true;
			this->updateController = true;
			this->updateCrossPosition();	
			this->targetController->camera = this->battlefieldModel->bfData->viewport->camera;
			this->targetController->skin = tank->skin;
			this->targetController->cameraController->setAnchorObject(tank->skin->turretMesh);
			Vector3 localPosition;
			WeaponCommonData *weaponCommonData = this->commonModel->getCommonData(tankData->turret);
			getBarrelOrigin(&localPosition, weaponCommonData);
			this->targetController->cameraController->setLocalPosition(&localPosition);
			this->targetController->cameraController->setElevation(0);
			this->targetController->cameraController->setElevationDirection(0);
			this->targetController->enter(param1);

#ifdef GRAPHICS
			Stage *stage = this->battlefieldModel->bfData->viewport->getStage();
			stage->addImage(this->image);

			Tank *tank = tankData->tank;

			tank->title()->hide();

			BitmapData *indicator = tank->title()->getTexture();

			if (this->indicatorBitmap == nullptr)
			{
				this->indicatorBitmap = stage->createImage(indicator, 5);
			}
			else
			{
				if (this->indicatorBitmap->data != indicator || this->indicatorBitmap->width != indicator->width()
					|| this->indicatorBitmap->height != indicator->height())
				{
					delete this->indicatorBitmap;
					this->indicatorBitmap = stage->createImage(indicator, 5);
				}
			}

			this->indicatorIndexFrame = tank->title()->indexFrame();

			this->indicatorBitmap->setX((stage->stageWidth() - this->indicatorBitmap->width) >> 1);
			this->indicatorBitmap->setY((stage->stageHeight() / 2) + (this->indicatorBitmap->height * 2));
			stage->addImage(this->indicatorBitmap);
#endif
			this->lock = true;
			this->fired = false;
			this->lockCheckIntersection = true;
			this->exitTime = -1;
			this->sendStartAimCommand();
		}
	}
	else
	{
		if (this->quickShot)
		{
			int _loc3_ = this->getEnergy(param1);
			if (_loc3_ == 1000)
			{
				this->onQuickShot();
			}
			this->quickShot = false;
		}
		else
		{
			if (this->fired)
			{
				this->activatedTime = 0;
				if (this->exitTime > 0)
				{
					if (param1 >= this->exitTime)
					{
						if (this->aim == true)
						{
							this->aim = false;
#ifdef GRAPHICS
							Stage *stage = this->battlefieldModel->bfData->viewport->getStage();
							stage->removeImage(this->image);
							stage->removeImage(this->indicatorBitmap);
							Tank *tank = tankData->tank;
							tank->title()->show();
#endif
							this->battlefieldModel->activateFollowCamera();
							this->battlefieldModel->resetFollowCamera();
							this->targetController->exit();
							this->updateController = false;
							this->lock = false;
							this->lockCheckIntersection = false;
							this->fired = false;
						}
					}
				}
			}
			else
			{
				if (this->aim == true)
				{
					this->setEnergyMode(ShaftEnergyMode::RECHARGE);
					this->performAimedShot(1000);
					this->exitTime = param1 + 500;
					this->fired = true;
				}
			}
		}
	}

	if (this->updateController)
	{
		this->updateCrossPosition();
#ifdef GRAPHICS
		Stage *stage = this->battlefieldModel->bfData->viewport->getStage();
		this->indicatorBitmap->x = ((stage->stageWidth() - this->indicatorBitmap->width) >> 1);
		this->indicatorBitmap->y = ((stage->stageHeight() / 2) + (this->indicatorBitmap->height * 2));
#endif
		this->targetController->update(param1, param2);

		if ((!((!((this->tank->leftTrack->lastContactsNum == 0) && (this->tank->rightTrack->lastContactsNum == 0))) || (!(this->hasIntersection())))))
		{
			this->forceExit(param1);
		}
	}

	return this->getStatus();
}

void ShaftModel::forceExit(int time)
{
	if (this->aim == true)
	{
		this->aim = false;
#ifdef GRAPHICS
		Stage *stage = this->battlefieldModel->bfData->viewport->getStage();
		stage->removeImage(this->image);
		stage->removeImage(this->indicatorBitmap);
		this->tank->title()->show();
#endif
		this->activatedTime = 0;
		this->battlefieldModel->activateFollowCamera();
		this->battlefieldModel->resetFollowCamera();
		this->targetController->exit();
		this->updateController = false;
		this->lock = false;
		this->lockCheckIntersection = false;
		this->fired = false;
		this->doSetEnergyMode(ShaftEnergyMode::RECHARGE, MyMath::min(this->getEnergy(time), 0), time);
		this->targetController->shaftMode = ShaftModes::TARGET_DEACTIVATION;
	}
}

void ShaftModel::sendQuickShotCommand(ShaftShotResult *result)
{
	bool array_first = true;

	QString str;

	str.append("battle;quick_shot_shaft;{\"targets\":[");


	for (const TankData *tankData : result->targets)
	{
		if (array_first == true)
			array_first = false;
		else
			str.append(QChar(','));

		str.append("{\"target_id\":\"");
		str.append(*tankData->userName);
		str.append("\"}");
	}

	str.append("],\"dir\":{\"y\":");
	str.append(double_to_string(result->dir.y));
	str.append(",\"z\":");
	str.append(double_to_string(result->dir.z));
	str.append(",\"x\":");
	str.append(double_to_string(result->dir.x));
	str.append("},\"hitPoints\":[");

	int len = result->hitPoints.length();
	for (int i = 0; i < len; i++)
	{
		if (i != 0)
			str.append(QChar(','));

		const Vector3 *vector = result->hitPoints.at(i);

		str.append("{\"y\":");
		str.append(double_to_string(vector->y));
		str.append(",\"z\":");
		str.append(double_to_string(vector->z));
		str.append(",\"x\":");
		str.append(double_to_string(vector->x));
		str.append("}");
	}

	str.append("]}");

	Service::getNetworker()->send(str);
}

void ShaftModel::sendStartAimCommand()
{
	Service::getNetworker()->send("battle;start_fire");
}

void ShaftModel::sendStopAimCommand(int physTime, Vector3 *staticShot, QVector<Aim*> *aims)
{
	QString str;
	str.append("battle;fire;{\"static_shot\":");

	if (staticShot == nullptr)
	{
		str.append("null");
	}
	else
	{
		str.append("{\"w\":0,\"x\":");
		str.append(double_to_string(staticShot->x));
		str.append(",\"y\":");
		str.append(double_to_string(staticShot->y));
		str.append(",\"z\":");
		str.append(double_to_string(staticShot->z));
		str.append(",\"length\":1,\"lengthSquared\":1}");
	}

	str.append(",\"targets\":[");

	bool array_first = true;

	for (const Aim *aim : *aims)
	{
		if (array_first == true)
			array_first = false;
		else
			str.append(",");

		Vector3 pointPos;
		pointPos.copy(&aim->targetHitPoint);
		globalToLocal(aim->target, &pointPos);

		str.append("{\"target\":{\"point_pos\":{\"w\":0,\"x\":");
		str.append(double_to_string(pointPos.x));
		str.append(",\"y\":");
		str.append(double_to_string(pointPos.y));
		str.append(",\"z\":");
		str.append(double_to_string(pointPos.z));
		str.append(",\"length\":1,\"lengthSquared\":1},\"pos\":{\"w\":0,\"x\":");
		str.append(double_to_string(aim->targetHitPoint.x));
		str.append(",\"y\":");
		str.append(double_to_string(aim->targetHitPoint.y));
		str.append(",\"z\":");
		str.append(double_to_string(aim->targetHitPoint.z));
		str.append(",\"length\":1,\"lengthSquared\":1},\"id\":\"");
		str.append(*aim->target->tankData->userName);
		str.append("\"}}");
	}

	str.append("],\"phys_time\":");
	str.append(QString::number(physTime));
	str.append(",\"energy\":");
	str.append(double_to_string(this->getEnergy(this->battlefieldModel->bfData->physTime)));
	str.append("}");

	Service::getNetworker()->send(str);
}

void ShaftModel::onQuickShot()
{
	TankData *tankData = TankData::localTankData;
	WeaponCommonData *commonData = this->commonModel->getCommonData(tankData->turret);

	Vector3 _xAxis;
	Vector3 _barrelOrigin;

	WeaponUtils::calculateGunParams(tankData->tank->skin->turretMesh,
									commonData->getMuzzle(0),
									&allGunParams->muzzlePosition,
									&_barrelOrigin,
									&_xAxis,
									&allGunParams->direction);




	QHash<Body*, TankData*> *tanks = (QHash<Body*, TankData*>*)this->battlefieldModel->getBattlefieldData()->tanks;

	this->quickShotTargetSystem->getTargets(tankData,
											&_barrelOrigin,
											&allGunParams->direction,
											&_xAxis,
											tanks,
											this->shotResult);

	if (this->shotResult->hitPoints.length() == 0)
	{
		//trace("shot v nikuda");
	}
	else
	{
		if (this->shotResult->targets.length() == 0)
		{
			//trace("shot on static");
			this->createStaticShotEffect(tankData->user, this->shotResult->hitPoints.at(0), &allGunParams->direction);
			this->battlefieldModel->addDecal(this->shotResult->hitPoints.at(0), &allGunParams->muzzlePosition, 50, decalMaterial.get(), RotationState::NONE);
		}
		else
		{
			int i = 0;
			while (i < this->shotResult->targets.length())
			{
				TankData *target = this->shotResult->targets[i];
				Tank *currTank = target->tank;
				this->createTankShotEffect(tankData->user, this->shotResult->hitPoints.at(i), &allGunParams->direction);
				currTank->addWorldForceScaled(this->shotResult->hitPoints.at(i), &allGunParams->direction, this->shaftData->impactQuickShot);
				i++;
			}

			if (this->shotResult->hitPoints.length() > this->shotResult->targets.length())
			{
				const Vector3 *v = this->shotResult->hitPoints.at(this->shotResult->targets.length());
				this->createStaticShotEffect(tankData->user, v, &allGunParams->direction);
				this->battlefieldModel->addDecal(v, &allGunParams->muzzlePosition, 50, decalMaterial.get(), RotationState::NONE);
			}
		}
	}
#ifdef GRAPHICS
	ShaftSFXModel *shaftSFX = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(TankData::localTankData->turret->getId()));
	shaftSFX->createMuzzleFlashEffect(tankData->turret, &tankData->tank->skin->turretDescriptor->shaftMuzzle, tankData->tank->skin->turretMesh);
	shaftSFX->createShotSoundEffect(tankData->turret, tankData->user, &allGunParams->muzzlePosition);
#endif
	TankData::localTankData->tank->addWorldForceScaled(&allGunParams->muzzlePosition, &allGunParams->direction, -commonData->kickback);
	this->doSetEnergyMode(ShaftEnergyMode::RECHARGE, 0, this->battlefieldModel->bfData->physTime);
	this->sendQuickShotCommand(this->shotResult);
}


void ShaftModel::quickFire(ClientObjectUser *user, const QStringList &targets, QVector<Vector3*> *hitPoints, const Vector3 *dir)
{
	(void)dir;

	TankData *_loc5_ = this->tankModel->getTankData(user);

	if (_loc5_ == TankData::localTankData)
		return;

	if (this->commonModel == nullptr)
		return;

	WeaponCommonData *_loc6_ = this->commonModel->getCommonData(_loc5_->turret);
	ShaftData *shaftData = ClientObjectWeaponShaft::data(_loc5_->turret);


	WeaponUtils::calculateGunParamsAux(_loc5_->tank->skin->turretMesh,
									   _loc6_->getMuzzle(0),
									   &allGunParams->muzzlePosition, //out
									   &allGunParams->direction);
#ifdef GRAPHICS
	ShaftSFXModel *shaftSFX = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(_loc5_->turret->getId()));
	shaftSFX->createMuzzleFlashEffect(_loc5_->turret, &_loc5_->tank->skin->turretDescriptor->shaftMuzzle, _loc5_->tank->skin->turretMesh);
#endif
	_loc5_->tank->addWorldForceScaled(&allGunParams->muzzlePosition, &allGunParams->direction, -_loc6_->kickback);
	if (hitPoints->length() == 0)
	{
		return;
	}

	if (targets.length() == 0)
	{
		const Vector3 *v = hitPoints->at(0);
		this->createStaticShotEffect(_loc5_->user, v, &allGunParams->direction);
		this->battlefieldModel->addDecal(v, &allGunParams->muzzlePosition, 50, decalMaterial.get(), RotationState::NONE);
	}
	else
	{
		int _loc10_ = 0;
		while (_loc10_ < targets.length())
		{
			const Vector3 *_loc11_ = hitPoints->at(_loc10_);
			QString target_id = targets.at(_loc10_);
			ClientObjectUser *user = Service::getBattleController()->tanks->value(target_id);
			TankData *tankData = this->tankModel->getTankData(user);
			Tank *tank = tankData->tank;
			this->createTankShotEffect(_loc5_->user, _loc11_, &allGunParams->direction);
			tank->addWorldForceScaled(_loc11_, &allGunParams->direction, shaftData->impactQuickShot);
			_loc10_++;
		}

		if (hitPoints->length() > targets.length())
		{
			int l = targets.length();
			const Vector3 *_loc9_ = hitPoints->at(l);
			this->createStaticShotEffect(_loc5_->user, _loc9_, &allGunParams->direction);
			this->battlefieldModel->addDecal(_loc9_, &allGunParams->muzzlePosition, 50, decalMaterial.get(), RotationState::NONE);
		}
	}
}

void ShaftModel::fire(ClientObjectUser *user, const Vector3 *staticHit, QVector<ServerShaftTargetData*> *targets)
{
	TankData *firringTank = this->tankModel->getTankData(user);
	if (firringTank == TankData::localTankData)
	{
		return;
	}

	if (this->commonModel == nullptr)
		return;

	WeaponCommonData *commonData = this->commonModel->getCommonData(firringTank->turret);
	WeaponUtils::calculateGunParamsAux(firringTank->tank->skin->turretMesh,
									   commonData->getMuzzle(0),
									   &allGunParams->muzzlePosition,
									   &allGunParams->direction);
#ifdef GRAPHICS
	ShaftSFXModel *effects = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(firringTank->turret->getId()));
	effects->createMuzzleFlashEffect(firringTank->turret, &firringTank->tank->skin->turretDescriptor->shaftMuzzle, firringTank->tank->skin->turretMesh);
#endif
	int i = 0;

	while (i < targets->length())
	{
		ServerShaftTargetData *target = targets->at(i);
		ClientObjectUser *user = Service::getBattleController()->tanks->value(target->targetId);
		TankData *targetTank = this->tankModel->getTankData(user);
		if (targetTank != nullptr)
		{
			//Vector3 *targetTankPos = targetTank.tank.state.pos;
		}
		localToGlobal(targetTank->tank, &target->globalHitPoint);
		this->createTankShotEffect(firringTank->user, &target->globalHitPoint, &allGunParams->direction);
		targetTank->tank->addWorldForceScaled(&target->globalHitPoint, &allGunParams->direction, commonData->impactForce);
		i++;
	}

	if (staticHit != nullptr)
	{
		this->createStaticShotEffect(firringTank->user, staticHit, &allGunParams->direction);
		this->battlefieldModel->addDecal(staticHit, &allGunParams->muzzlePosition, 50, decalMaterial.get(), RotationState::NONE);
	}
#ifdef GRAPHICS
	effects->createShotSoundEffect(firringTank->turret, firringTank->user, &allGunParams->muzzlePosition);
#endif
}

void ShaftModel::performAimedShot(double param1)
{
	(void)param1;

	Vector3 _direction;

	WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(TankData::localTankData->turret);

#ifdef GRAPHICS
	ShaftSFXModel *shaftSFX = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(TankData::localTankData->turret->getId()));
	shaftSFX->stopManualSound(TankData::localTankData->user);
#endif

	int physTime = this->battlefieldModel->bfData->physTime;

	this->localTankData->tank->skin->turretMesh->composeMatrix();

	getAllGunParams(this->localTankData->tank, allGunParams, commonData);


	AimedShotResult *result = this->getAimedShotTargets();
	this->targetController->cameraController->readCameraDirection(&_direction);
	//double _loc4_ = (param1 - this->getEnergy(physTime));



	for (int i = 0; i < result->aims.length(); i++)
	{
		Aim *a = result->aims.at(i);
		this->createTankShotEffect(TankData::localTankData->user, &a->targetHitPoint, &_direction);
		a->target->addWorldForceScaled(&a->targetHitPoint, &_direction, commonData->impactForce);
	}



	if (result->isStatic == true)
	{
		this->createStaticShotEffect(TankData::localTankData->user, &result->staticHitPoint, &_direction);
		this->battlefieldModel->addDecal(&result->staticHitPoint, &allGunParams->muzzlePosition, 50, decalMaterial.get(), RotationState::NONE);
	}


#ifdef GRAPHICS
	shaftSFX->createShotSoundEffect(TankData::localTankData->turret, TankData::localTankData->user, &allGunParams->muzzlePosition);
#endif
	TankData::localTankData->tank->addWorldForceScaled(&allGunParams->muzzlePosition, &allGunParams->direction, -commonData->kickback);



	this->sendStopAimCommand(physTime, &result->staticHitPoint, &result->aims);
	this->doSetEnergyMode(ShaftEnergyMode::RECHARGE, MyMath::min(this->getEnergy(physTime), 0), physTime);
	this->targetController->shaftMode = ShaftModes::TARGET_DEACTIVATION;
}

void ShaftModel::createTankShotEffect(ClientObjectUser *user, const Vector3 *point, Vector3 *dir)
{
	TankData *tankData = this->tankModel->getTankData(user);

	if (this->commonModel == nullptr)
		return;


	WeaponCommonData *commonData = this->commonModel->getCommonData(tankData->turret);

	WeaponUtils::calculateGunParamsAux(tankData->tank->skin->turretMesh,
									   commonData->getMuzzle(0),
									   &allGunParams->muzzlePosition,
									   &allGunParams->direction);

#ifdef GRAPHICS
	ShaftSFXModel *effects = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(tankData->turret->getId()));

	effects->createHitPointsGraphicEffects(tankData->turret,
										   tankData->user,
										   nullptr,
										   point,
										   &allGunParams->muzzlePosition,
										   &allGunParams->direction,
										   dir);
#else
	(void)point;
	(void)dir;
#endif
}

void ShaftModel::createStaticShotEffect(ClientObjectUser *user, const Vector3 *point, Vector3 *dir)
{
	TankData *tankData = this->tankModel->getTankData(user);

	if (this->commonModel == nullptr)
		return;

	WeaponCommonData *commonData = this->commonModel->getCommonData(tankData->turret);
	WeaponUtils::calculateGunParamsAux(tankData->tank->skin->turretMesh, commonData->getMuzzle(0),
									   &allGunParams->muzzlePosition, &allGunParams->direction);

#ifdef GRAPHICS
	ShaftSFXModel *effects = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(tankData->turret->getId()));
	effects->createHitPointsGraphicEffects(tankData->turret, tankData->user, point, nullptr, &allGunParams->muzzlePosition, &allGunParams->direction, dir);
#else
	(void)point;
	(void)dir;
#endif
}





static bool raycast(BattlefieldModel *battlefieldModel,
					const Vector3 *origin,
					const Vector3 *direction,
					Object3DFilter *filter,
					Camera3D *camera,
					RayIntersectionData *ray)
{
	Object3DContainer *container = battlefieldModel->bfData->viewport->get_MapContainer();
	if (container == nullptr)
		return false;

	if (container->intersectRay(origin, direction, filter, camera, ray))
	{
		Object3D *object = ray->object;
		while (object != nullptr && object->mouseEnabled == false)
		{
			object = object->getParent();
		}
		ray->object = object;
		return true;
	}
	else
	{
		return false;
	}
}




bool ShaftModel::hasIntersection()
{
	WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(TankData::localTankData->turret);

	this->localTankData->tank->skin->turretMesh->composeMatrix();
	getAllGunParams(this->localTankData->tank, allGunParams, commonData);
	Vector3 *tankPos = &TankData::localTankData->tank->state.pos;

	Vector3 _direction;
	_direction.diff(&allGunParams->barrelOrigin, tankPos);
	this->exclusionSetController->addTemporaryItem(this->localTankData->tank->skin->turretMesh);
	this->exclusionSetController->addTemporaryItem(this->localTankData->tank->skin->hullMesh);
	RayIntersectionData rayData;
	bool rr = raycast(battlefieldModel, tankPos, &_direction, nullptr, nullptr, &rayData);

	if (rr != false && rayData.time <= 1)
	{
		Object3D *object = rayData.object;
		this->exclusionSetController->deleteAllTemporaryItems();
		return object->getName() == Object3DNames::STATIC;
	}
	this->exclusionSetController->deleteAllTemporaryItems();

	return false;
}

AimedShotResult* ShaftModel::getAimedShotTargets()
{
	AimedShotResult *result = new AimedShotResult();

	Vector3 _origin;
	Vector3 _direction;

	this->targetController->cameraController->readCameraPosition(&_origin);
	this->targetController->cameraController->readCameraDirection(&_direction);
	this->addTankSkinToExclusionSet(this->localTankData->tank->skin);

	for (;;)
	{
		RayIntersectionData data;
		if (raycast(this->battlefieldModel, &_origin, &_direction, &this->exclusionSetFilter, nullptr, &data))
		{
			Object3D *aim = data.object;

			Vector3 hitPoint;
			hitPoint.copy(&_origin);
			hitPoint.addScaled(data.time + 0.1, &_direction);

			QString name = aim->getName();

			if (name == Object3DNames::STATIC)
			{
				result->setStaticHitPoint(&hitPoint);
				break;
			}

			if (name == Object3DNames::TANK_PART || name == "turret" || name == "hull")
			{
				Tank *target = this->battlefieldModel->getObjects2Tank()->get(aim);
				if (target != nullptr)
				{
					if (this->isValidHit(target, aim, &hitPoint))
					{
						result->setTarget(target, &hitPoint);
					}
					this->addTankSkinToExclusionSet(target->skin);
				}
			}
			else
			{
				this->exclusionSetController->addTemporaryItem(aim);
			}
			_origin.copy(&hitPoint);
		}
		else
		{
			break;
		}
	}

	this->exclusionSetController->deleteAllTemporaryItems();

	return result;
}



bool ShaftModel::isValidHit(Tank *tank, Object3D *param2, Vector3 *param3)
{
	TankSkin *tankSkin = tank->skin;
	if (tankSkin->turretMesh == param2)
	{
		Matrix4 _m;
		Vector3 _p;
		Vector3 _closestBarrelOrigin;
		_m.setMatrix(&param2->position, &param2->rotation);
		_m.transformVectorInverse(param3, &_p);
		WeaponCommonData *commonData = this->weaponCommonModel->getCommonData(tank->tankData->turret);
		getClosestBarrelOrigin(&_p, commonData, &_closestBarrelOrigin);
		_m.transformVector(&_closestBarrelOrigin, &_p);
		_p.subtract(param3);

		if (this->battlefieldModel->bfData->collisionDetector->hasStaticHit(param3, &_p, CollisionGroup::STATIC, 1))
		{
			return false;
		}
	}
	return tankSkin->hullMesh->alpha == 1;
}


void ShaftModel::addTankSkinToExclusionSet(TankSkin *tankSkin)
{
	this->exclusionSetController->addTemporaryItem(tankSkin->hullMesh);
	this->exclusionSetController->addTemporaryItem(tankSkin->turretMesh);
}

void ShaftModel::targetModeOn()
{
	this->tankModel->lockControls(true);
	this->tankModel->applyControlState(TankData::localTankData, 0, true);
	this->tankModel->setControlState(TankData::localTankData, 0, true);
#ifdef GRAPHICS
	ShaftSFXModel *shaftSFX = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(TankData::localTankData->turret->getId()));
	shaftSFX->createManualModeEffects(TankData::localTankData->turret, TankData::localTankData->user, TankData::localTankData->tank->skin->turretMesh);
#endif
}

double ShaftModel::getStatus()
{
	int physTime = this->battlefieldModel->bfData->physTime;
	return this->getEnergy(physTime) / this->shaftData->maxEnergy;
}

void ShaftModel::doSetEnergyMode(ShaftEnergyMode mode, double energy, int physTime)
{
	this->energyMode = mode;
	switch (mode)
	{
		case ShaftEnergyMode::RECHARGE:
			this->energyAdditive = 0;
			this->energySpeed = this->shaftData->chargeRate;
			this->energyBaseTime = (physTime - ((energy / this->energySpeed) * thousand));
			break;
		case ShaftEnergyMode::DRAIN:
			this->energyAdditive = this->shaftData->maxEnergy;
			this->energySpeed = -(this->shaftData->dischargeRate);
			this->energyBaseTime = (physTime + (((this->shaftData->maxEnergy - energy) / this->energySpeed) * thousand));
			this->sendBeginEnergyDrain(physTime);
			break;
	}
}


void ShaftModel::sendBeginEnergyDrain(int time)
{
	Service::getNetworker()->send("battle;begin_enegry_drain;" + QString::number(time));
}


double ShaftModel::getEnergy(double energyTime)
{
	double enegry = this->energyAdditive + (double((energyTime - this->energyBaseTime) * this->energySpeed) / thousand);
	double c = MathUtils::clamp(enegry, 0, this->shaftData->maxEnergy);
	return c;
}

void ShaftModel::updateCrossPosition()
{
#ifdef GRAPHICS
	Stage *stage = this->battlefieldModel->bfData->viewport->getStage();
	int x = ((stage->stageWidth() - this->image->width) >> 1);
	int y = ((stage->stageHeight() - this->image->height) >> 1);
	this->image->setPosition(x, y);
#endif
}

void ShaftModel::stopEffects(TankData *ownerTankData)
{
	if (TankData::localTankData == ownerTankData)
	{
		if (this->aim == true)
		{
			this->aim = false;
#ifdef GRAPHICS
			Stage *stage = this->battlefieldModel->bfData->viewport->getStage();
			stage->removeImage(this->image);
			stage->removeImage(this->indicatorBitmap);
			Tank *tank = TankData::localTankData->tank;
			tank->title()->show();
#endif
			this->battlefieldModel->activateFollowCamera();
			this->battlefieldModel->resetFollowCamera();
			this->targetController->exit();
			this->setEnergyMode(ShaftEnergyMode::RECHARGE);
			this->targetController->deactivationTask->stop();
		}
		this->updateController = false;
		this->lock = false;
		this->lockCheckIntersection = false;
		this->fired = false;
		this->battlefieldModel->onResize();
	}
}

void ShaftModel::reset()
{
	this->doSetEnergyMode(ShaftEnergyMode::RECHARGE, this->shaftData->maxEnergy, this->battlefieldModel->bfData->physTime);
	if (TankData::localTankData == nullptr)
		return;

#ifdef GRAPHICS
	ShaftSFXModel *shaftSFX = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(TankData::localTankData->turret->getId()));
	shaftSFX->stopManualSound(TankData::localTankData->user);
#endif
	this->lockCheckIntersection = false;
}


void ShaftModel::setLocalUser(TankData *localUserData)
{
	this->objectLoaded(nullptr);
	this->localTankData = localUserData;
	this->localShotData = WeaponsManager::shotData(localUserData->turret->getId());
	this->localWeaponCommonData = this->weaponCommonModel->getCommonData(localUserData->turret);

	this->quickShotTargetSystem->setParams(this->battlefieldModel->getBattlefieldData()->physicsScene->collisionDetector,
										   this->localShotData->autoAimingAngleUp,
										   this->localShotData->numRaysUp,
										   this->localShotData->autoAimingAngleDown,
										   this->localShotData->numRaysDown,
										   1, nullptr);


	this->shaftData = ClientObjectWeaponShaft::data(localUserData->turret);


#ifdef GRAPHICS
	BitmapData *b = ShaftIndicator::getIndicator(localUserData->turret);

	Stage *stage = this->battlefieldModel->bfData->viewport->getStage();
	this->image = stage->createImage(b, 5);
#endif

	this->targetController = new TargetingController(this);
	this->reset();
}

void ShaftModel::clearLocalUser()
{

}

void ShaftModel::activateWeapon(int time)
{
	(void)time;
	this->activate = true;
}

void ShaftModel::deactivateWeapon(int time, bool sendServerCommand)
{
	(void)time;
	(void)sendServerCommand;
	this->activate = false;
}

TargetingController* ShaftModel::getTargetingController()
{
	return this->targetController;
}

bool ShaftModel::Filter::check(Object3D *object) //true = ignore
{
	return this->exclusionSet->value(object);
}
