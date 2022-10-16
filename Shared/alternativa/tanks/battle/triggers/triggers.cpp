#include "triggers.h"
#include "../DeferredAction.h"
#include "../Trigger.h"
#include "deferredtriggeraddition.h"
#include "deferredtriggerdeletion.h"


Triggers::Triggers()
{
	running = false;
}

void Triggers::executeDeferredActions()
{
	while (!this->deferredActions.isEmpty())
		this->deferredActions.takeLast()->execute();
}

void Triggers::add(Trigger *trigger)
{
	if (this->running == true)
	{
		this->deferredActions.append(new DeferredTriggerAddition(this, trigger));
	}
	else
	{
		if (this->_triggers.indexOf(trigger) < 0)
		{
			this->_triggers.append(trigger);
		}
	}
}

void Triggers::remove(Trigger *trigger)
{
	if (this->running)
	{
		this->deferredActions.append(new DeferredTriggerDeletion(this, trigger));
	}
	else
	{
		int last = this->_triggers.length();
		if (last > 0)
		{
			int i = this->_triggers.indexOf(trigger);
			if (i >= 0)
			{
				this->_triggers[i] = this->_triggers[--last];
				this->_triggers.takeLast();
			}
		}
	}
}

void Triggers::check(Body *body)
{
	if (body == nullptr)
		return;

	this->running = true;
	int length = this->_triggers.length();
	for (int i = 0; i < length; i++)
		this->_triggers.at(i)->checkTrigger(body);
	this->running = false;
	this->executeDeferredActions();
}
