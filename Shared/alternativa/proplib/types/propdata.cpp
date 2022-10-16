#include "propdata.h"
#include "propstate.h"
#include <QHashIterator>


PropData::PropData(const QString &name)
{
	this->_name = name;
}

QString PropData::getName()
{
	return this->_name;
}

void PropData::addState(const QString &stateName, std::shared_ptr<PropState> state)
{
	this->states[stateName] = state;
}

PropState* PropData::getStateByName(const QString &stateName)
{
	return this->states[stateName].get();
}

PropState* PropData::getDefaultState()
{
	return this->states.value(PropState::DEFAULT_NAME).get();
}

QString PropData::toString()
{
	return "[Prop name=" + this->_name + "]";
}

void PropData::traceProp()
{
	QHashIterator<QString, std::shared_ptr<PropState>> i(states);
	while (i.hasNext())
	{
		i.value()->traceState();
	}
}
