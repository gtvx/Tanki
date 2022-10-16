#ifndef WEAPONCOMMONDATA_H
#define WEAPONCOMMONDATA_H

#include <qglobal.h>
#include <memory>

class IWeaponController;
class Vector3;

class WeaponCommonData
{
	std::shared_ptr<QVector<std::shared_ptr<Vector3>>> _muzzles;
public:
	double kickback;
	double impactCoeff;
	double impactForce;
	double turretRotationAccel;
	double turretRotationSpeed ;
	int currBarrel;
	int muzzle_length;

	IWeaponController *weaponController;

	void setMuzzles(std::shared_ptr<QVector<std::shared_ptr<Vector3>>> &muzzles);

	const Vector3* getMuzzle(int index);

	int muzzleLength()
	{
		return muzzle_length;
	}

	WeaponCommonData();
};

#endif // WEAPONCOMMONDATA_H
