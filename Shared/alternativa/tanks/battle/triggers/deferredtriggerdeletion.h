#ifndef DEFERREDTRIGGERDELETION_H
#define DEFERREDTRIGGERDELETION_H

#include "../DeferredAction.h"

class Triggers;
class Trigger;

class DeferredTriggerDeletion : public DeferredAction
{
	Triggers *triggers;
	Trigger *trigger;

public:
	DeferredTriggerDeletion(Triggers *triggers, Trigger *trigger);
	void execute();
};

#endif // DEFERREDTRIGGERDELETION_H
