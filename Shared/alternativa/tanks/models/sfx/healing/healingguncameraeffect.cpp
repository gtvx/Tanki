#include "healingguncameraeffect.h"
#include "mymath.h"
#include "flash/utils/gettimer.h"
#include "alternativa/math/vector3.h"

static const double amplitude = 2; //2, 0, 1000
static const double frequency = 25; //25, 0, 1000


HealingGunCameraEffect::HealingGunCameraEffect()
{

}

bool HealingGunCameraEffect::update(int time, int delta, Vector3 *position, Vector3 *rotation)
{
	(void)delta;
	(void)rotation;

	if (this->refCounter <= 0)
		return false;

	double dt = 0.001 * (time - this->startTime);
	position->z = position->z + (amplitude * MyMath::sin(frequency * dt));
	return true;
}

void HealingGunCameraEffect::onAddedToController(IFollowCameraController *controller)
{
	(void)controller;
	this->startTime = getTimer();
}

void HealingGunCameraEffect::destroy()
{

}
