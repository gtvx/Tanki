#include "localhulltransformupdater.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"

LocalHullTransformUpdater::LocalHullTransformUpdater(Tank *tank)
{
	this->tank = tank;
}

LocalHullTransformUpdater::~LocalHullTransformUpdater()
{

}

void LocalHullTransformUpdater::reset()
{

}

void LocalHullTransformUpdater::update(double)
{
	Matrix3 m3;
	this->tank->interpolatedOrientation.toMatrix3(&m3);
	Vector3 position;
	position.copy(&this->tank->skinCenterOffset);
	position.transformBy3(&m3);
	position.add(&this->tank->interpolatedPosition);
	Vector3 eulerAngles;
	this->tank->interpolatedOrientation.getEulerAngles(&eulerAngles);
	this->tank->skin->updateHullTransform(&position, &eulerAngles);
}
