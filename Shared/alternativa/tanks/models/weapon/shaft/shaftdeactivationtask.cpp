#include "shaftdeactivationtask.h"
#include "mymath.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/logic/battlelogicunits.h"
#include "service.h"


static const double FOV_SPEED = (5 * 0.001);//0.005
static const double ALPHA_SPEED = (5 * 0.001);//0.005
static const double DEFAULT_TARGET_FOV = (MyMath::PI / 2);//1.5707963267949

ShaftDeactivationTask::ShaftDeactivationTask(GameCamera *camera)
{
	this->camera = camera;
	this->targetFov = DEFAULT_TARGET_FOV;
	this->battle = Service::getBattlefieldModel();
}

void ShaftDeactivationTask::setSkin(TankSkin *skin)
{
	this->skin = skin;
}

void ShaftDeactivationTask::setTargetFov(double value)
{
	this->targetFov = value;
}

void ShaftDeactivationTask::start()
{
	this->battle->getBattleLogicUnits()->addLogicUnit(this);
}

void ShaftDeactivationTask::stop()
{
	this->battle->getBattleLogicUnits()->removeLogicUnit(this);
}

void ShaftDeactivationTask::runLogic(int time, int param2)
{
	(void)time;

	this->camera->fov = (this->camera->fov + (FOV_SPEED * param2));
	if (this->camera->fov > this->targetFov)
	{
		this->camera->fov = this->targetFov;
	}
	double v = this->skin->hullMesh->alpha;
	v = v + (ALPHA_SPEED * param2);
	if (v > 1)
	{
		v = 1;
	}
	this->skin->setAlpha(v);
	if (this->camera->fov == this->targetFov && v == 1)
	{
		this->stop();
	}
}
