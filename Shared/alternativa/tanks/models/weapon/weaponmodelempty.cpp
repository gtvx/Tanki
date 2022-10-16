#include "weaponmodelempty.h"

WeaponModelEmpty::WeaponModelEmpty()
{

}

WeaponModelEmpty::~WeaponModelEmpty()
{

}

void WeaponModelEmpty::stopEffects(TankData *tankData)
{
	(void)tankData;
}

void WeaponModelEmpty::reset()
{

}

void WeaponModelEmpty::setLocalUser(TankData *tankData)
{
	(void)tankData;
}

void WeaponModelEmpty::clearLocalUser()
{

}

void WeaponModelEmpty::activateWeapon(int time)
{
	(void)time;
}

void WeaponModelEmpty::deactivateWeapon(int time, bool sendServerCommand)
{
	(void)time;
	(void)sendServerCommand;
}

double WeaponModelEmpty::update(int time, int deltaTime)
{
	(void)time;
	(void)deltaTime;
	return 0.;
}
