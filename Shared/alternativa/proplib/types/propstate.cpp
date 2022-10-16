#include "propstate.h"
#include "alternativa/proplib/objects/propobject.h"
#include "alternativa/proplib/types/proplod.h"
#include <QDebug>

const QString PropState::DEFAULT_NAME = "default";



PropState::PropState()
{

}

void PropState::addLOD(std::shared_ptr<PropObject> &prop, double distance)
{
	if (prop == nullptr) {
		qDebug() << "error PropState::addLOD null";
		return;
	}

	this->_lods.append(std::make_shared<PropLOD>(prop, distance));
}

int PropState::getNumLODs()
{
	return this->_lods.length();
}

std::shared_ptr<PropLOD> PropState::lodByIndex(int index)
{
	return this->_lods[index];
}

std::shared_ptr<PropObject> PropState::getDefaultObject()
{
	if (this->_lods.length() == 0)
	{
		//throw new Error("No LODs found");
		throw 1412343;
	}
	return this->_lods[0]->prop;
}

void PropState::traceState()
{
	for (std::shared_ptr<PropLOD> &lod : this->_lods)
	{
		lod->traceLOD();
	}
}
