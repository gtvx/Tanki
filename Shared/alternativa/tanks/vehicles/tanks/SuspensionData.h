#ifndef SUSPENSIONDATA_H
#define SUSPENSIONDATA_H

class SuspensionData
{
public:
	double rayLength = 0;
	double rayOptimalLength = 0;
	double forceOffset = 0;
	double dampingCoeff = 0;
	double smallVel = 100;
	double staticFriction = 4;
	double dynamicFriction = 0.2;
};

#endif // SUSPENSIONDATA_H
