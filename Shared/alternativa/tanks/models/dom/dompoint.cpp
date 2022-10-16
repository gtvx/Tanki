#include "dompoint.h"
#include "dommodel.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"


DomPoint::DomPoint(const DomPointID &id, const Vector3 *pos, double radius, DOMModel *model)
{
	this->capture = false;
	this->serverProgress = 0.;
	this->clientProgress = 0.;
	this->progressSpeed = 0.;
	this->pos.copy(pos);
	this->radius = radius;
	this->model = model;
	this->id = id;
}

DomPoint::~DomPoint()
{

}


void DomPoint::tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff)
{
	(void)time;
	(void)deltaMsec;
	(void)deltaSec;
	(void)interpolationCoeff;

	if (model->userTankData == nullptr)
		return;

	if (model->userTankData->spawnState == TankSpawnState::ACTIVE)
	{
		double dot = this->pos.distanceTo(&model->userTankData->tank->state.pos);
		if (this->capture == false)
		{
			if (dot <= this->radius)
			{
				this->capture = true;
				this->model->tankCapturingPoint(this, model->userTankData);
			}
		}
		else
		{
			if (dot > this->radius)
			{
				this->capture = false;
				this->model->tankLeaveCapturingPoint(this, model->userTankData);
			}
		}
	}
	else
	{
		if (this->capture == true)
		{
			this->capture = false;
			this->model->tankLeaveCapturingPoint(this, model->userTankData);
		}
	}
}

void DomPoint::readPos(Vector3 *v)
{
	v->copy(&this->pos);
}
