#ifndef IWEAPON_H
#define IWEAPON_H

class ClientObjectWeapon;
class IWeaponController;
class ClientObjectUser;

class IWeapon
{
public:
	virtual void ownerLoaded(ClientObjectUser*) = 0;
	virtual void ownerUnloaded(ClientObjectUser*) = 0;
	virtual void ownerDisabled(ClientObjectUser*) = 0;
	virtual double update(int, int) = 0;
	virtual void reset() = 0;
	virtual void enable() = 0;
	virtual void disable() = 0;
	virtual void stop() = 0;
	virtual double getTurretRotationAccel(ClientObjectWeapon*) = 0;
	virtual double getTurretRotationSpeed(ClientObjectWeapon*) = 0;
	virtual IWeaponController* getWeaponController() = 0;
	virtual ~IWeapon() {}
};

#endif // IWEAPON_H
