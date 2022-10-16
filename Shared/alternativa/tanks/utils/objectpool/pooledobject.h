#ifndef POOLEDOBJECT_H
#define POOLEDOBJECT_H

class ObjectPool;

class PooledObject
{
protected:
	ObjectPool *objectPool;

public:
	PooledObject(ObjectPool *objectPool, int id);
	virtual ~PooledObject();
	void storeInPool();
};

#endif // POOLEDOBJECT_H
