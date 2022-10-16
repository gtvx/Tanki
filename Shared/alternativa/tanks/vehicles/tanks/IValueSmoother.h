#ifndef IVALUESMOOTHER_H
#define IVALUESMOOTHER_H


class IValueSmoother
{
public:
	virtual void reset(double) = 0;
	virtual double update(double) = 0;
	virtual void setTargetValue(double) = 0;
	virtual double getTargetValue() = 0;
	virtual ~IValueSmoother() {}
};

#endif // IVALUESMOOTHER_H
