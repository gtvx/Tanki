#ifndef ICOMMONTARGETEVALUATOR_H
#define ICOMMONTARGETEVALUATOR_H

class Body;

class ICommonTargetEvaluator
{
public:
	virtual double getTargetPriority(Body*, double, double) = 0;
	virtual ~ICommonTargetEvaluator() {}
};

#endif // ICOMMONTARGETEVALUATOR_H
