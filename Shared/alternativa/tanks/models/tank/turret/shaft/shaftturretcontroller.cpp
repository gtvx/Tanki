#include "shaftturretcontroller.h"
#include "service.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/models/tank/tankmodel.h"


ShaftTurretController::ShaftTurretController(TankData *tankData)
{
	playSound = true;
	this->tankData = tankData;
	this->tankModel = Service::getTankModel();
	//tankData.sounds.playTurretSound(false);
}

void ShaftTurretController::rotateTurret(double deltaSec)
{
	Tank *tank = this->tankData->tank;
	int turretRotationDir = ((((this->tankData->ctrlBits & TankModel::TURRET_LEFT) || (this->tankData->ctrlBits & TankModel::LEFT)) >> 4) - (((this->tankData->ctrlBits & TankModel::TURRET_RIGHT) || (this->tankData->ctrlBits & TankModel::RIGHT)) >> 5));
	int turretRotationDirAddition = (((this->tankData->ctrlBits & TankModel::LEFT) >> 2) - ((this->tankData->ctrlBits & TankModel::RIGHT) >> 3));
	if (turretRotationDir == 0)
	{
		turretRotationDir = turretRotationDirAddition;
	}
	if (turretRotationDir != 0)
	{
		if ((this->tankData->ctrlBits & TankModel::CENTER_TURRET) != 0)
		{
			this->tankData->ctrlBits = (this->tankData->ctrlBits & (~(TankModel::CENTER_TURRET)));
			if (this->tankData->local)
			{
				this->tankModel->controlBits = (this->tankModel->controlBits & (~(TankModel::CENTER_TURRET)));
			}
			if (tank->getTurretDirSign() == turretRotationDir)
			{
				tank->stopTurret();
				//this->tankData.sounds.playTurretSound(false);
			}
		}
		tank->rotateTurret((turretRotationDir * deltaSec), false);
	}
	else
	{
		if ((this->tankData->ctrlBits & TankModel::CENTER_TURRET) != 0)
		{
			if (tank->rotateTurret((-(tank->getTurretDirSign()) * deltaSec), true))
			{
				this->tankData->ctrlBits = this->tankData->ctrlBits & (~(TankModel::CENTER_TURRET));
				tank->stopTurret();
			}
		}
		else
		{
			tank->stopTurret();
		}
	}
	if (this->playSound)
	{
		//this->tankData.sounds.playTurretSound((!(this->tankData.tank.turretTurnSpeed == 0)));
	}
}

void ShaftTurretController::enableTurretSound(bool value)
{
	this->playSound = value;
}
