#ifndef SHOTDATA_H
#define SHOTDATA_H


class ShotData
{
public:
	double autoAimingAngleDown;
	double autoAimingAngleUp;
	int numRaysUp;
	int numRaysDown;
	int reloadMsec;

	ShotData(int reloadMsec)
	{
		this->reloadMsec = reloadMsec;
	}
};

#endif // SHOTDATA_H
