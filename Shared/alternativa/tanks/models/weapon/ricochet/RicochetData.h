#ifndef RICOCHETDATA_H
#define RICOCHETDATA_H

class RicochetData
{
public:
	double shotRadius;
	double shotSpeed;
	int energyCapacity;
	int energyPerShot;
	double energyRechargeSpeed;
	double shotDistance;

	RicochetData(double shotRadius,
				 double shotSpeed,
				 int energyCapacity,
				 int energyPerShot,
				 int energyRechargeSpeed,
				 double shotDistance)
	{
		this->shotRadius = shotRadius;
		this->shotSpeed = shotSpeed;
		this->energyCapacity = energyCapacity;
		this->energyPerShot = energyPerShot;
		this->energyRechargeSpeed = energyRechargeSpeed;
		this->shotDistance = shotDistance;
	}
};

#endif // RICOCHETDATA_H
