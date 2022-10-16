#include "objectpool.h"

#include "pooledobject.h"

ObjectPool::ObjectPool()
{

}

ObjectPool::~ObjectPool()
{
	clear();
}

void ObjectPool::clear()
{
	for (PooledObject * o: list)
	{
		delete o;
	}

	list.clear();
}

void ObjectPool::storeInPool(PooledObject *o)
{
	list.append(o);
}
