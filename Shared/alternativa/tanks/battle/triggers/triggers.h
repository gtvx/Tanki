#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <QVector>

class Trigger;
class DeferredAction;
class Body;


class Triggers
{
	QVector<Trigger*> _triggers;
	QVector<DeferredAction*> deferredActions;
	bool running;

	void executeDeferredActions();

public:
	Triggers();

	void add(Trigger *trigger);
	void remove(Trigger *trigger);
	void check(Body *body);
};

#endif // TRIGGERS_H
