#ifndef FREEZEDATA_H
#define FREEZEDATA_H


class FreezeData
{
public:

	double damageAreaConeAngle;
	double damageAreaRange;
	int energyCapacity;
	int energyDischargeSpeed;
	int energyRechargeSpeed;
	int weaponTickMsec;

	FreezeData(double damageAreaConeAngle,
			   double damageAreaRange,
			   int energyCapacity,
			   int energyDischargeSpeed,
			   int energyRechargeSpeed,
			   int weaponTickMsec)
	{
		this->damageAreaConeAngle = damageAreaConeAngle;
		this->damageAreaRange = damageAreaRange;
		this->energyCapacity = energyCapacity;
		this->energyDischargeSpeed = energyDischargeSpeed;
		this->energyRechargeSpeed = energyRechargeSpeed;
		this->weaponTickMsec = weaponTickMsec;
	}
};

#endif // FREEZEDATA_H
