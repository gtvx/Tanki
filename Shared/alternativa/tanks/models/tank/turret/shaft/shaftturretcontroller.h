#ifndef SHAFTTURRETCONTROLLER_H
#define SHAFTTURRETCONTROLLER_H

#include "../TurretController.h"

class TankData;
class TankModel;

class ShaftTurretController : public TurretController
{
	TankData *tankData;
	TankModel *tankModel;
	bool playSound;

public:
	ShaftTurretController(TankData *tankData);
	void rotateTurret(double deltaSec) override;
	void enableTurretSound(bool value);
};

#endif // SHAFTTURRETCONTROLLER_H
