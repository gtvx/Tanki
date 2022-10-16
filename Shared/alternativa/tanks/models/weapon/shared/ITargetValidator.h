#ifndef ITARGETVALIDATOR_H
#define ITARGETVALIDATOR_H

class Body;

class ITargetValidator
{
public:
	virtual bool isValidTarget(Body*) = 0;
	virtual void destroy() = 0;
};

#endif // ITARGETVALIDATOR_H
