#include "valuesmoother.h"

ValueSmoother::ValueSmoother(double smoothingSpeedUp, double smoothingSpeedDown, double targetValue, double currentValue)
{
	this->smoothingSpeedUp = smoothingSpeedUp;
	this->smoothingSpeedDown = smoothingSpeedDown;
	this->targetValue_ = targetValue;
	this->currentValue = currentValue;
}

ValueSmoother::~ValueSmoother()
{

}

void ValueSmoother::reset(double value)
{
	this->currentValue = value;
	this->targetValue_ = value;
}

double ValueSmoother::update(double dt)
{
	if (this->currentValue < this->targetValue_)
	{
		this->currentValue += this->smoothingSpeedUp * dt;
		if (this->currentValue > this->targetValue_)
		{
			this->currentValue = this->targetValue_;
		}
	}
	else
	{
		if (this->currentValue > this->targetValue_)
		{
			this->currentValue -= this->smoothingSpeedDown * dt;
			if (this->currentValue < this->targetValue_)
			{
				this->currentValue = this->targetValue_;
			}
		}
	}
	return this->currentValue;
}

void ValueSmoother::setTargetValue(double v)
{
	this->targetValue_ = v;
}

double ValueSmoother::getTargetValue()
{
	return this->targetValue_;
}
