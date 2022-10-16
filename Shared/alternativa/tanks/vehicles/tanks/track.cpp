#include "track.h"
#include "suspensionray.h"
#include "tank.h"


Track::Track(Tank *tank, int raysNum, Vector3 *relPos, double trackLength)
{
	this->lastContactsNum = 0;
	this->tank = tank;
	this->bot_turn_add_value = 0.;
	this->setTrackParams(raysNum, relPos, trackLength);
}

Track::~Track()
{
	for (uint32_t i = 0; i < raysNum; i++)
	{
		delete this->rays[i];
	}
}

void Track::setTrackParams(int raysNum, Vector3 *relPos, double trackLength)
{
	this->raysNum = raysNum;
	//this->rays = new Vector.<SuspensionRay>(raysNum);
	double step = (trackLength / double(raysNum - 1));

	if (raysNum > 10)
		throw 514255;

	for (int i = 0; i < raysNum; i++)
	{
		Vector3 _relPos(relPos->x, ((relPos->y + (0.5 * trackLength)) - (i * step)), relPos->z);
		Vector3 _relDir(0, 0, -1);

		this->rays[i] = new SuspensionRay(this->tank, &_relPos, &_relDir);
	}
}

void Track::setCollisionGroup(int value)
{
	for (uint32_t i = 0; i < this->raysNum; i++)
		this->rays[i]->collisionGroup = value;
}


void Track::addForces(double dt, double throttle, double maxSpeed, int slipTerm, double weight, SuspensionData *data, bool brake)
{
	if (tank->isLocal() == true)
	{
		throttle += bot_turn_add_value;
		bot_turn_add_value = 0.;
	}

	//0 0 860 0 1.1e+06 0
	//123000 123000 860 0 1.1e+06 0

	//0.03 0 860 0 "1100000" 0

	this->lastContactsNum = 0;
	for (uint32_t i = 0; i < this->raysNum; i++)
	{
		if (this->rays[i]->calculateIntersection(data->rayLength))
		{
			this->lastContactsNum++;
		}
	}

	if (this->lastContactsNum == 0)
		return;


	double springCoeff = (0.5 * weight) / (this->lastContactsNum * data->rayOptimalLength);
	throttle = throttle * (this->raysNum / this->lastContactsNum);


	int mid = (int)(this->raysNum >> 1);
	if (this->raysNum & (1 == 0))
	{
		for (int i = 0; i < (int)this->raysNum; i++)
		{
			this->rays[i]->addForce(dt, throttle, maxSpeed, ((i <= mid) ? slipTerm : -(slipTerm)), springCoeff, data, brake);
		}
	}
	else
	{
		for (int i = 0; i < (int)this->raysNum; i++)
		{
			this->rays[i]->addForce(dt, throttle, maxSpeed, ((i < mid) ? slipTerm : ((i > mid) ? -(slipTerm) : 0)), springCoeff, data, brake);
		}
	}
}
