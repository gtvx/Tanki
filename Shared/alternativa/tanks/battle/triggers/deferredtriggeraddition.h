#ifndef DEFERREDTRIGGERADDITION_H
#define DEFERREDTRIGGERADDITION_H

#include "../DeferredAction.h"

class Triggers;
class Trigger;


class DeferredTriggerAddition : public DeferredAction
{
	Triggers *triggers;
	Trigger *trigger;
public:

	DeferredTriggerAddition(Triggers *triggers, Trigger *trigger);
	void execute();
};

#endif // DEFERREDTRIGGERADDITION_H
