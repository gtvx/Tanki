#include "defaultturretcontroller.h"
#include "service.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"


DefaultTurretController::DefaultTurretController(TankData *tankData)
{
	this->tankData = tankData;
	this->tankModel = Service::getTankModel();
}

void DefaultTurretController::rotateTurret(double deltaSec)
{
	Tank *tank = this->tankData->tank;
	int turretRotationDir = ((this->tankData->ctrlBits & TankModel::TURRET_LEFT) >> 4) - ((this->tankData->ctrlBits & TankModel::TURRET_RIGHT) >> 5);
	if (turretRotationDir != 0)
	{
		if ((this->tankData->ctrlBits & TankModel::CENTER_TURRET) != 0)
		{
			this->tankData->ctrlBits = this->tankData->ctrlBits & ~TankModel::CENTER_TURRET;
			if (this->tankData->local)
			{
				this->tankModel->controlBits = this->tankModel->controlBits & ~TankModel::CENTER_TURRET;
			}
			if (tank->getTurretDirSign() == turretRotationDir)
			{
				tank->stopTurret();
				//this->tankData->sounds.playTurretSoundfalse;
			}
		}
		tank->rotateTurret(turretRotationDir * deltaSec, false);
	}
	else
	{
		if ((this->tankData->ctrlBits & TankModel::CENTER_TURRET) != 0)
		{
			if (tank->rotateTurret(-tank->getTurretDirSign() * deltaSec, true))
			{
				this->tankData->ctrlBits = this->tankData->ctrlBits & ~(TankModel::CENTER_TURRET);
				tank->stopTurret();
			}
		}
		else
		{
			tank->stopTurret();
		}
	}

	//this->tankData.sounds.playTurretSound((!(this->tankData.tank.turretTurnSpeed == 0)));

}

void DefaultTurretController::enableTurretSound(bool value)
{
	(void)value;
}
