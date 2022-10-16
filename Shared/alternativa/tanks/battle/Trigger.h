#ifndef TRIGGER_H
#define TRIGGER_H

class Body;

class Trigger
{
public:
	 virtual void checkTrigger(Body*) = 0;
};

#endif // TRIGGER_H
