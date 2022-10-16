#ifndef TANKEVENTDISPATCHER_H
#define TANKEVENTDISPATCHER_H

#include "ITankEventDispatcher.h"
#include <QHash>
#include <QVector>

class TankEventDispatcher : public ITankEventDispatcher
{
	bool processingEvent;
	QHash<int, QVector<ITankEventListener*>*> eventListeners;
	QHash<int, QVector<ITankEventListener*>*> addedListeners;
	QHash<int, QVector<ITankEventListener*>*> removedListeners;

public:
	TankEventDispatcher();
	virtual ~TankEventDispatcher();

	void addTankEventListener(int eventType, ITankEventListener *listener);
	void removeTankEventListener(int eventType, ITankEventListener *listener);
	void dispatchEvent(int eventType, TankData *tankData);

private:
	void addListener(QHash<int, QVector<ITankEventListener*>*> *storage, int eventType, ITankEventListener *listener);
	void removeListener(QHash<int, QVector<ITankEventListener*>*> *storage, int eventType, ITankEventListener *listener);
	void processDeferredActions();
};

#endif // TANKEVENTDISPATCHER_H
