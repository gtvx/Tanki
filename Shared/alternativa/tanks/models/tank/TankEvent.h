#ifndef TANKEVENT_H
#define TANKEVENT_H

class TankEvent
{
public:
	enum
	{
		LOADED = 1,
		KILLED = 2,
		UNLOADED = 3,
		SPAWNED = 4,
		ACTIVATED = 5,
	};
};

#endif // TANKEVENT_H
