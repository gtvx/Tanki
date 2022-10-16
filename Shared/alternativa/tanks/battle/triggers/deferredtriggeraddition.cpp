#include "deferredtriggeraddition.h"
#include "triggers.h"

DeferredTriggerAddition::DeferredTriggerAddition(Triggers *triggers, Trigger *trigger)
{
	this->triggers = triggers;
	this->trigger = trigger;
}

void DeferredTriggerAddition::execute()
{
	this->triggers->add(this->trigger);
}
