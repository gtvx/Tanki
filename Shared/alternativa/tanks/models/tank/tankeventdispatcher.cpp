#include "tankeventdispatcher.h"
#include "ITankEventListener.h"


TankEventDispatcher::TankEventDispatcher()
{

}

TankEventDispatcher::~TankEventDispatcher()
{

}

void TankEventDispatcher::addTankEventListener(int eventType, ITankEventListener *listener)
{
	if (this->processingEvent)
	{
		this->removeListener(&this->removedListeners, eventType, listener);
		this->addListener(&this->addedListeners, eventType, listener);
	}
	else
	{
		this->addListener(&this->eventListeners, eventType, listener);
	}
}

void TankEventDispatcher::removeTankEventListener(int eventType, ITankEventListener *listener)
{
	if (this->processingEvent)
	{
		this->removeListener(&this->addedListeners, eventType, listener);
		this->addListener(&this->removedListeners, eventType, listener);
	}
	else
	{
		this->removeListener(&this->eventListeners, eventType, listener);
	}
}

void TankEventDispatcher::dispatchEvent(int eventType, TankData *tankData)
{
	this->processingEvent = true;
	QVector<ITankEventListener*> *listeners = this->eventListeners.value(eventType);
	if (listeners != nullptr)
	{
		for (ITankEventListener *item : *listeners)
		{
			item->handleTankEvent(eventType, tankData);
		}
	}
	this->processingEvent = false;
	this->processDeferredActions();
}

void TankEventDispatcher::addListener(QHash<int, QVector<ITankEventListener*>*> *storage, int eventType, ITankEventListener *listener)
{
	QVector<ITankEventListener*> *listeners = storage->value(eventType);
	if (listeners == nullptr)
	{
		listeners = new QVector<ITankEventListener*>();
		storage->insert(eventType, listeners);
	}
	else
	{
		int idx = listeners->indexOf(listener);
		if (idx > -1)
			return;
	}
	listeners->append(listener);
}

void TankEventDispatcher::removeListener(QHash<int, QVector<ITankEventListener*>*> *storage, int eventType, ITankEventListener *listener)
{
	QVector<ITankEventListener*> *listeners = storage->value(eventType);
	if (listeners == nullptr)
		return;

	int idx = listeners->indexOf(listener);
	if (idx < 0)
		return;

	if (listeners->length() == 1)
	{
		storage->remove(eventType);
		//delete storage[eventType];
	}
	else
	{
		listeners->removeAt(idx);
		//listeners.splice(idx, 1);
	}
}

void TankEventDispatcher::processDeferredActions()
{
	QHashIterator<int, QVector<ITankEventListener*>*> i(this->removedListeners);
	while (i.hasNext()) {
		i.next();
		int eventType = i.key();
		QVector<ITankEventListener*> *listeners = i.value();

		for (ITankEventListener *item : *listeners)
		{
			this->removeListener(&this->eventListeners, eventType, item);
		}
	}


	QHashIterator<int, QVector<ITankEventListener*>*> i2(this->addedListeners);
	while (i2.hasNext()) {
		i2.next();
		int eventType = i2.key();
		QVector<ITankEventListener*> *listeners = i2.value();
		for (ITankEventListener *item : *listeners)
		{
			this->addListener(&this->eventListeners, eventType, item);
		}
	}

	this->removedListeners.clear();
	this->addedListeners.clear();

	/*
	for (key : this->removedListeners)
	{
		int eventType = key;
		QVector.<ITankEventListener*> *listeners = this->removedListeners[key];
		delete this->removedListeners[key];
		int len = listeners.length;
		int i = 0;
		while (i < len)
		{
			this->removeListener(this->eventListeners, eventType, listeners[i]);
			i++;
		}
	}
	for (key in this->addedListeners)
	{
		int eventType = key;
		QVector.<ITankEventListener*> *listeners = this->addedListeners[key];
		delete this->addedListeners[key];
		int len = listeners.length;
		int i = 0;
		while (i < len)
		{
			this->addListener(this->eventListeners, eventType, listeners[i]);
			i++;
		}
	}
	*/
}
