#include "deferredtriggerdeletion.h"
#include "triggers.h"

DeferredTriggerDeletion::DeferredTriggerDeletion(Triggers *triggers, Trigger *trigger)
{
	this->triggers = triggers;
	this->trigger = trigger;
}

void DeferredTriggerDeletion::execute()
{
	this->triggers->remove(this->trigger);
}
