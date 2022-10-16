#include "landingcontroller.h"
#include "bonusmesh.h"
#include "mymath.h"
#include "parabonus2.h"
#include "bonustrigger.h"

static const double ANGULAR_SPEED = 2.5;


LandingController::LandingController(ParaBonus2 *bonus)
{
	this->bonus = bonus;
	normal.reset();
	pivot.reset();
	r.reset();
	axis.reset();
	angle = 0;
}

void LandingController::init(Vector3 *pivot, Vector3 *normal)
{
	this->pivot.copy(pivot);
	this->normal.copy(normal);
}

void LandingController::start()
{
	BonusMesh *mesh = this->bonus->skin.get();
	this->r.copy(&mesh->position);
	this->r.subtract(&this->pivot);
	this->axis.copy(&Vector3::Z_AXIS);
	this->axis.cross(&this->normal);
	this->axis.normalize();
	this->angle = MyMath::acos(this->normal.z);
	this->newState.position.copy(&mesh->position);
	this->newState.orientation.setFromEulerAnglesXYZ(mesh->rotation.x, mesh->rotation.y, mesh->rotation.z);
	this->oldState.copy(&this->newState);
}

void LandingController::runBeforePhysicsUpdate(double dt)
{
	dt = dt * 0.001;
	this->oldState.copy(&this->newState);
	double deltaAngle = ANGULAR_SPEED * dt;

	if (deltaAngle > this->angle)
	{
		deltaAngle = this->angle;
		this->angle = 0.;
	}
	else
	{
		this->angle = this->angle - deltaAngle;
	}

	Matrix3 m;
	m.fromAxisAngle(&this->axis, deltaAngle);
	this->r.transformBy3(&m);
	this->newState.position.copy(&this->pivot);
	this->newState.position.add(&this->r);

	this->newState.orientation.addScaledVector(&this->axis, deltaAngle);
	this->updateTrigger();

	if (this->angle == 0.)
	{
		this->interpolatePhysicsState(1);
		this->render();
		this->bonus->onLandingComplete();
	}
}

void LandingController::updateTrigger()
{
	Matrix3 m;
	this->newState.orientation.toMatrix3(&m);
	this->bonus->trigger->setTransform(&this->newState.position, &m);
}

void LandingController::interpolatePhysicsState(double interpolationCoeff)
{
	this->interpolatedState.interpolate(&this->oldState, &this->newState, interpolationCoeff);
}

void LandingController::render()
{
	BonusMesh *mesh = this->bonus->skin.get();
	mesh->position.copy(&this->interpolatedState.position);
	this->interpolatedState.orientation.getEulerAngles(&mesh->rotation);
}
