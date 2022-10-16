#ifndef ITANKEVENTLISTENER_H
#define ITANKEVENTLISTENER_H

class TankData;

class ITankEventListener
{
public:
	virtual void handleTankEvent(int, TankData*) = 0;
};

#endif // ITANKEVENTLISTENER_H
