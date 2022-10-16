#ifndef DEFAULTTURRETCONTROLLER_H
#define DEFAULTTURRETCONTROLLER_H

#include "../TurretController.h"

class TankData;
class TankModel;

class DefaultTurretController : public TurretController
{
	TankData *tankData;
	TankModel *tankModel;
public:
	DefaultTurretController(TankData *tankData);
	void rotateTurret(double deltaSec);
	void enableTurretSound(bool value);
};

#endif // DEFAULTTURRETCONTROLLER_H
