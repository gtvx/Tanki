#ifndef WEAPONMODELEMPTY_H
#define WEAPONMODELEMPTY_H

#include "IWeaponController.h"

class WeaponModelEmpty : public IWeaponController
{
public:
	WeaponModelEmpty();
	virtual ~WeaponModelEmpty();

	void stopEffects(TankData *tankData) override;
	void reset() override;
	void setLocalUser(TankData *tankData) override;
	void clearLocalUser() override;
	void activateWeapon(int time) override;
	void deactivateWeapon(int time, bool sendServerCommand) override;
	double update(int time, int deltaTime) override;
};

#endif // WEAPONMODELEMPTY_H
