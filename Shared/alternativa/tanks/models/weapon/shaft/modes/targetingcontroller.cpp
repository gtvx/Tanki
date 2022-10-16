#include "targetingcontroller.h"
#include "../ShaftData.h"
#include "../shaftmodel.h"
#include "../ShaftEnergyMode.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/math/vector3.h"
#include "alternativa/math/quaternion.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "../states/ShaftModes.h"
#include "../cameracontrollers/targetingcameracontroller.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "../shaftdeactivationtask.h"
#include "alternativa/tanks/utils/mathutils.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidablegraphicobjects.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/tank/turret/TurretController.h"
#include "alternativa/tanks/models/tank/turret/default/defaultturretcontroller.h"
#include "alternativa/tanks/models/tank/turret/shaft/shaftturretcontroller.h"
#include "alternativa/object/clientobjectweapon.h"
#include "tanks/weaponsmanager.h"
#include "alternativa/tanks/camera/camerafovcalculator.h"
#include "../sfx/shaftsfxmodel.h"
#include "mymath.h"
#include "service.h"



TargetingController::TargetingController(ShaftModel *shaftModel)
{
	this->stateDuration = 500;
	this->targetFOV = 0.698132;
	this->timeLeft = 0;

	this->shaftMode = ShaftModes::TARGET_ACTIVATION;

	this->deactivationTask = nullptr;

	ShaftData *shaftData = shaftModel->shaftData;
	this->targetFOV = shaftData->minimumFOV;
	this->fovInterpolator.setInterval(shaftData->initialFOV, shaftData->minimumFOV);
	this->rangeInterpolator.setInterval(shaftData->shrubsHidingRadiusMin, shaftData->shrubsHidingRadiusMax);
	this->cameraController = new TargetingCameraController(shaftData->elevationAngleUp,
														   shaftData->elevationAngleDown,
														   shaftData->verticalTargetingSpeed,
														   shaftData->horizontalTargetingSpeed);

	this->initialCameraOrientation = new Quaternion();
	this->targetCameraOrientation = new Quaternion();
	//this->targetCameraOrientation = Quaternion.createFromAxisAngle(Vector3.X_AXIS, (-(Math.PI) / 2));
	this->targetCameraOrientation->setFromAxisAngle(&Vector3::X_AXIS, (-MyMath::PI) / 2.);



	this->initialCameraPosition = new Vector3();
	this->cameraPosition = new Vector3();
	this->cameraOrientation = new Quaternion();
	this->cameraMatrix = new Matrix4();
	this->turretMatrix = new Matrix4();
	this->cameraAngles = new Vector3();
	this->reticleAlphaInterpolator.setInterval(0, 1);
	this->shaftModel = shaftModel;
	this->targetCameraPosition = new Vector3(0, 30, 10);
	this->tankModel = Service::getTankModel();
}

void TargetingController::enter(int time)
{
	Tank *tank = TankData::localTankData->tank;

	this->savedTurnAcceleration = tank->turretTurnAcceleration;
	this->savedTurnSpred = tank->maxTurretTurnSpeed;
	tank->turretTurnAcceleration = 2;

	this->setTurretSpeedFactor(this->savedTurnSpred);
	this->up = (!(MathUtils::getBitValue(tank->getTurretDirSign(), 1) == 0));
	this->down = (!(MathUtils::getBitValue(tank->getTurretDirSign(), 2) == 0));
	this->shaftModel->setEnergyMode(ShaftEnergyMode::DRAIN);
	this->timeLeft = this->stateDuration;
	this->alphaInterpolator.setInterval(this->skin->hullMesh->alpha, 0);
	this->fovInterpolator.setInterval(this->camera->fov, this->targetFOV);
	this->calculateCameraInitialValues();
	this->exitTime = -1;
	this->fired = false;
	this->chargingEffectActive = true;
	this->battlefieldModel = Service::getBattlefieldModel();
	this->deactivationTask = new ShaftDeactivationTask(this->battlefieldModel->bfData->viewport->camera);
	this->deactivationTask->setSkin(this->skin);
	this->deactivationTask->stop();
	this->shaftMode = ShaftModes::TARGET_ACTIVATION;
	this->cameraController->reload();
	this->battlefieldModel->initCameraController(this->cameraController);
	this->battlefieldModel->getHidableGraphicObjects()->enable();
	this->battlefieldModel->getHidableGraphicObjects()->setCenterAndRadius(&tank->state.pos, 0);
	this->initalEnegry = this->shaftModel->getEnergy(time);
	this->shaftModel->targetModeOn();
	this->tankModel->localUserData->tank->setTurretController(new ShaftTurretController(this->tankModel->localUserData));
	this->tankModel->localUserData->tank->getTurretController()->enableTurretSound(false);
	this->sfxModel = WeaponsManager::getShaftSFX(WeaponsManager::getObjectFor(tank->tankData->turret->getId()));
}

void TargetingController::exit()
{
	//Main.stage.stageWidth, Main.stage.stageHeight
	int Width = 1000;
	int Height = 1000;

	this->deactivationTask->setTargetFov(CameraFovCalculator::getCameraFov(Width, Height));
	this->deactivationTask->start();
	TankData::localTankData->tank->turretTurnAcceleration = this->savedTurnAcceleration;
	TankData::localTankData->tank->setMaxTurretTurnSpeed(this->savedTurnSpred, true);
	this->tankModel->localUserData->tank->setTurretController(new DefaultTurretController(this->tankModel->localUserData));
	this->tankModel->localUserData->tank->getTurretController()->enableTurretSound(true);
#ifdef GRAPHICS
	this->sfxModel->playTargetingSound(this->tankModel->localUserData->turret, this->tankModel->localUserData->user, false);
#endif
	this->battlefieldModel->getHidableGraphicObjects()->disnable();
	this->battlefieldModel->getHidableGraphicObjects()->restore();
}

void TargetingController::setTurretSpeedFactor(double value)
{
	TankData::localTankData->tank->setMaxTurretTurnSpeed((value * 0.5), false);
	this->cameraController->setMaxElevationSpeedFactor(value);
}

void TargetingController::update(int param1, int param2)
{
	switch (this->shaftMode)
	{
		case ShaftModes::TARGET_ACTIVATION:
			if (this->timeLeft <= 0)
			{
				this->shaftMode = ShaftModes::TARGETING;
				this->fovInterpolator.setInterval(this->camera->fov, this->targetFOV);
			}
			else
			{
				this->timeLeft = this->timeLeft - param2;
				double v = 1 - (double(this->timeLeft) / double(this->stateDuration));
				if (v > 1)
					v = 1;

				this->skin->setAlpha(this->alphaInterpolator.interpolate(v));
				this->updateCamera(v);
			}
			break;
		case ShaftModes::TARGETING:
		{
			double weaponEnegry = this->shaftModel->getEnergy(param1);
			double en = ((this->initalEnegry - weaponEnegry) / 1000);
			this->lastEnergy = en;
			this->cameraController->setCameraFov(this->fovInterpolator.interpolate(en));
			if (((weaponEnegry == 0) && (this->chargingEffectActive)))
			{
				this->chargingEffectActive = false;
#ifdef GRAPHICS
				this->sfxModel->fadeChargingEffect(TankData::localTankData->turret, TankData::localTankData->user);
#endif
			}
			this->up = (TankData::localTankData->ctrlBits & TankModel::FORWARD);
			this->down = (TankData::localTankData->ctrlBits & TankModel::BACK);
			int dir = (int(this->up) - int(this->down));
#ifdef GRAPHICS
			this->sfxModel->playTargetingSound(TankData::localTankData->turret, TankData::localTankData->user, (((!(dir == 0)) && (!(this->cameraController->isMaxElevation()))) || (!(this->tankModel->localUserData->tank->turretTurnSpeed == 0))));
#endif
			this->cameraController->setElevationDirection(dir);
			this->battlefieldModel->getHidableGraphicObjects()->setCenterAndRadius(&TankData::localTankData->tank->state.pos, this->rangeInterpolator.interpolate(en));
			break;
		}
		case ShaftModes::TARGET_DEACTIVATION:
			break;
		default:
			break;
	}
}

void TargetingController::calculateCameraInitialValues()
{
	Object3D *mesh = this->skin->turretMesh;
	this->turretMatrix->setMatrix(&mesh->position, &mesh->rotation);
	this->cameraMatrix->setMatrix(&this->camera->position, &this->camera->rotation);
	this->turretMatrix->invert();
	this->cameraMatrix->append(this->turretMatrix);
	this->cameraMatrix->getEulerAngles(this->cameraAngles);
	this->initialCameraOrientation->setFromEulerAnglesXYZ(this->cameraAngles->x, this->cameraAngles->y, this->cameraAngles->z);
	this->cameraMatrix->getAxis(3, this->initialCameraPosition);
}

void TargetingController::updateCamera(double param1)
{
	this->cameraPosition->interpolate(param1, this->initialCameraPosition, this->targetCameraPosition);
	this->cameraOrientation->slerp(this->initialCameraOrientation, this->targetCameraOrientation, param1);
	this->cameraOrientation->toMatrix4(this->cameraMatrix);
	this->cameraMatrix->setPosition(this->cameraPosition);
	Object3D *mesh = this->skin->turretMesh;
	this->turretMatrix->setMatrix(&mesh->position, &mesh->rotation);
	this->cameraMatrix->append(this->turretMatrix);
	this->cameraMatrix->getEulerAngles(this->cameraAngles);
	this->camera->position.x = this->cameraMatrix->d;
	this->camera->position.y = this->cameraMatrix->h;
	this->camera->position.z = this->cameraMatrix->l;
	this->camera->rotation.x = this->cameraAngles->x;
	this->camera->rotation.y = this->cameraAngles->y;
	this->camera->rotation.z = this->cameraAngles->z;
}
