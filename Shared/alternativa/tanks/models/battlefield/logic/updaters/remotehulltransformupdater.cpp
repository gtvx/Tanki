#include "remotehulltransformupdater.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "mymath.h"



#define SMOOTHING_COEFF MyMath::PI / 10.4719



static double smoothValue(double param1, double param2, double param3, double param4, double param5)
{
	param1 = param1 + (param2 * param3);
	return param1 + ((param4 - param1) * param5);
}


RemoteHullTransformUpdater::RemoteHullTransformUpdater(Tank *tank)
{
	this->tank = tank;
}

void RemoteHullTransformUpdater::reset()
{
	BodyState *b = &this->tank->prevState;
	this->smoothedPosition.copy(&b->pos);
	this->smoothedOrientation.copy(&b->orientation);
}

void RemoteHullTransformUpdater::update(double param1)
{
	Body *body = this->tank;
	BodyState *body_state = &body->prevState;
	Vector3 *velocity = &body_state->velocity;
	velocity->x = velocity->y = velocity->z = 0;
	const Vector3 &_loc5_ = this->tank->interpolatedPosition;
	double _loc6_ = SMOOTHING_COEFF;
	this->smoothedPosition.x = smoothValue(this->smoothedPosition.x, velocity->x, param1, _loc5_.x, _loc6_);
	this->smoothedPosition.y = smoothValue(this->smoothedPosition.y, velocity->y, param1, _loc5_.y, _loc6_);
	this->smoothedPosition.z = smoothValue(this->smoothedPosition.z, velocity->z, param1, _loc5_.z, _loc6_);
	Vector3 *rotation = &body_state->rotation;
	rotation->x = rotation->y = rotation->z = 0;

	Matrix3 m3;
	Vector3 smoothedEulerAngles;

	this->smoothedOrientation.addScaledVector(rotation, param1);
	this->smoothedOrientation.slerp(&this->smoothedOrientation, &this->tank->interpolatedOrientation, _loc6_);
	this->smoothedOrientation.getEulerAngles(&smoothedEulerAngles);
	this->smoothedOrientation.toMatrix3(&m3);
	Vector3 position;
	position.copy(&this->tank->skinCenterOffset);
	position.transformBy3(&m3);
	position.add(&this->smoothedPosition);
	this->tank->skin->updateHullTransform(&position, &smoothedEulerAngles);
}
