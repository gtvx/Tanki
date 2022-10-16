#ifndef ITANKEVENTDISPATCHER_H
#define ITANKEVENTDISPATCHER_H

class ITankEventListener;
class TankData;

class ITankEventDispatcher
{
public:
	virtual void addTankEventListener(int, ITankEventListener*) = 0;
	virtual void removeTankEventListener(int, ITankEventListener*) = 0;
	virtual void dispatchEvent(int, TankData*) = 0;
};

#endif // ITANKEVENTDISPATCHER_H
