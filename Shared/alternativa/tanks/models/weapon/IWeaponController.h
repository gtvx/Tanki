#ifndef IWEAPONCONTROLLER_H
#define IWEAPONCONTROLLER_H

class TankData;

class IWeaponController
{
public:
	virtual void stopEffects(TankData *tankData) = 0;
	virtual void reset() = 0;
	virtual void setLocalUser(TankData *tankData) = 0;
	virtual void clearLocalUser() = 0;
	virtual void activateWeapon(int time) = 0;
	virtual void deactivateWeapon(int time, bool sendServerCommand) = 0;
	virtual double update(int time, int deltaTime) = 0;
};

#endif // IWEAPONCONTROLLER_H
