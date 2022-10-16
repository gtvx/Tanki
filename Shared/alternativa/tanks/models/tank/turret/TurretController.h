#ifndef TURRETCONTROLLER_H
#define TURRETCONTROLLER_H

class TurretController
{
public:
	virtual void rotateTurret(double) = 0;
	virtual void enableTurretSound(bool) = 0;
	virtual ~TurretController() {}
};

#endif // TURRETCONTROLLER_H
