#ifndef PROPLOD_H
#define PROPLOD_H

#include <memory>

class PropObject;

class PropLOD
{
public:
	std::shared_ptr<PropObject> prop;
	double distance;

	PropLOD(std::shared_ptr<PropObject> &prop, double distance)
	{
		this->prop = prop;
		this->distance = distance;
	}

	void traceLOD();
};

#endif // PROPLOD_H
