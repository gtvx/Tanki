#include "object3dcontainerproxy.h"
#include "alternativa/engine3d/core/object3dcontainer.h"


Object3DContainerProxy::Object3DContainerProxy(std::shared_ptr<Object3DContainer> container)
{
	 this->setContainer(container);
}

void Object3DContainerProxy::addChild(Object3D *o)
{
	if (o == nullptr)
	{
		throw 141342313;
		//throw (new ArgumentError("Parameter is null"));
	}
	this->objects.insert(o, true);
	this->container->addChild(o);
}

void Object3DContainerProxy::addChildAt(Object3D *o, int param2)
{
	if (o == nullptr)
	{
		throw 141342313;
		//throw (new ArgumentError("Parameter is null"));
	}
	this->objects.insert(o, true);
	this->container->addChildAt(o, param2);
}

void Object3DContainerProxy::addChildren(QVector<Object3D *> &list)
{
	for (Object3D *o : list)
	{
		this->addChild(o);
	}
}

void Object3DContainerProxy::removeChild(Object3D *o)
{
	if (o == nullptr)
	{
		throw 141342313;
		//throw (new ArgumentError("Parameter is null"));
	}
	if (this->objects.contains(o))
	{
		this->objects.remove(o);
		this->container->removeChild(o);
	}
}

void Object3DContainerProxy::setContainer(std::shared_ptr<Object3DContainer> container)
{
	this->container = container != nullptr ? container : new_Object3DContainer();
	QVector<Object3D*> list = this->removeAllChildren();
	this->addChildren(list);
}

QVector<Object3D*> Object3DContainerProxy::removeAllChildren()
{
	QVector<Object3D*> list;

	QHashIterator<Object3D*, bool> i(this->objects);

	while (i.hasNext()) {
		i.next();
		Object3D *o = i.key();
		this->container->removeChild(o);
		list.append(o);
	}

	this->objects.clear();

	return list;
}
