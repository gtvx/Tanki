#include "anglevalues.h"
#include "mymath.h"

AngleValues::AngleValues()
{

}

void AngleValues::init(double startAngle, double targetAngle, double accelerationCoeff)
{
	this->totalAngle = targetAngle - startAngle;
	if (this->totalAngle < 0)
	{
		this->totalAngle = -this->totalAngle;
		this->angleDirection = -1;
	}
	else
	{
		this->angleDirection = 1;
	}
	if (this->totalAngle > MyMath::PI)
	{
		this->angleDirection = -this->angleDirection;
		this->totalAngle = (2 * MyMath::PI) - this->totalAngle;
	}
	this->angularAcceleration = accelerationCoeff * this->totalAngle;
	this->angularSpeed = 0;
	this->currentAngle = 0;
}

void AngleValues::reverseAcceleration()
{
	this->angularAcceleration = -this->angularAcceleration;
}

double AngleValues::update(double dt)
{
	if (this->currentAngle < this->totalAngle)
	{
		double dv = this->angularAcceleration * dt;
		double delta = (this->angularSpeed + (0.5 * dv)) * dt;
		this->angularSpeed = this->angularSpeed + dv;
		double da = this->totalAngle - this->currentAngle;
		if (da < delta)
			delta = da;
		this->currentAngle = this->currentAngle + delta;
		return delta * this->angleDirection;
	}
	return 0;
}
