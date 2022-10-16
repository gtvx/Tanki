#include "blinker.h"

Blinker::Blinker(int initialInterval, int minInterval, int intervalDecrement, double minValue, double maxValue, double speedCoeff)
{
	this->initialInterval = initialInterval;
	this->minInterval = minInterval;
	this->intervalDecrement = intervalDecrement;
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->speedCoeff = speedCoeff;
	this->valueDelta = maxValue - minValue;
}

void Blinker::init(int now)
{
	this->value = this->maxValue;
	this->currInterval = this->initialInterval;
	this->speed = this->getSpeed(-1);
	this->switchTime = now + this->currInterval;
}

void Blinker::setMaxValue(double value)
{
	if (value < this->minValue)
	{
		return;
	}
	this->maxValue = value;
	this->valueDelta = this->maxValue - this->minValue;
}

void Blinker::setMinValue(double value)
{
	if (value > this->maxValue)
		return;
	this->minValue = value;
	this->valueDelta = this->maxValue - this->minValue;
}

double Blinker::updateValue(int now, int delta)
{
	this->value = this->value + (this->speed * delta);

	if (this->value > this->maxValue)
		this->value = this->maxValue;

	if (this->value < this->minValue)
		this->value = this->minValue;

	if (now >= this->switchTime)
	{
		if (this->currInterval > this->minInterval)
		{
			this->currInterval = this->currInterval - this->intervalDecrement;
			if (this->currInterval < this->minInterval)
			{
				this->currInterval = this->minInterval;
			}
		}

		this->switchTime = now + this->currInterval;
		if (this->speed < 0)
		{
			this->speed = this->getSpeed(1);
		}
		else
		{
			this->speed = this->getSpeed(-1);
		}
	}

	return this->value;
}

double Blinker::getSpeed(double direction)
{
	return ((direction * this->speedCoeff) * this->valueDelta) / this->currInterval;
}
