#ifndef VALUESMOOTHER_H
#define VALUESMOOTHER_H

#include "IValueSmoother.h"

class ValueSmoother : public IValueSmoother
{
public:
	double currentValue;
	double targetValue_;
	double smoothingSpeedUp;
	double smoothingSpeedDown;

	ValueSmoother(double smoothingSpeedUp, double smoothingSpeedDown, double targetValue, double currentValue);
	virtual ~ValueSmoother();
	void reset(double value);
	double update(double dt);
	void setTargetValue(double v);
	double getTargetValue();
};


#endif // VALUESMOOTHER_H
