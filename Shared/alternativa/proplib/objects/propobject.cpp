#include "propobject.h"

PropObject::PropObject(PropObjectType type)
{
	this->_type = type;
	//this->object = nullptr;
}

void PropObject::traceProp()
{
	//trace(this.object);
}

void PropObject::setObject3D(std::shared_ptr<Object3D> &object)
{
	this->object = object;
}

std::shared_ptr<Object3D> PropObject::getObject3D()
{
	return this->object;
}
