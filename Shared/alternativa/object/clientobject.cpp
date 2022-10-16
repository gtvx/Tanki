#include "clientobject.h"
#include <QString>

ClientObject::ClientObject(const QString &id, ClientClass *clientClass, const QString &name, ICommandHandler *handler)
{
	(void)handler;
	this->_id = new QString(id);
	this->_clientClass = clientClass;
	this->_name = new QString(name);
	//this->_handler = handler;
	//this->models = new Vector.<Long>();
	//this->initParams = new Dictionary();
	//this->runtimeParams = new Dictionary();
}

ClientObject::~ClientObject()
{
	delete this->_id;
	delete this->_name;
	this->_id = nullptr;
	this->_name = nullptr;
}

QString ClientObject::getId()
{
	return *_id;
}
