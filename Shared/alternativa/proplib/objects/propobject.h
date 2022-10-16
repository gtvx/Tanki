#ifndef PROPOBJECT_H
#define PROPOBJECT_H

class Object3D;

#include "PropObjectType.h"
#include <memory>

class PropObject
{
	PropObjectType _type;

public:

	std::shared_ptr<Object3D> object;


	void setObject3D(std::shared_ptr<Object3D>&);

	std::shared_ptr<Object3D> getObject3D();

	PropObject(PropObjectType type);

	PropObjectType getType()
	{
		return this->_type;
	}

	virtual void traceProp();
};

#endif // PROPOBJECT_H
