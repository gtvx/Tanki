#include "pooledobject.h"
#include "objectpool.h"
#include <QDebug>


PooledObject::PooledObject(ObjectPool *objectPool, int id)
{
	this->objectPool = objectPool;
	if (objectPool == nullptr)
	{
		qDebug() << "error PooledObject" << id;
	}
}

PooledObject::~PooledObject()
{

}

void PooledObject::storeInPool()
{
	this->objectPool->storeInPool(this);
}

