#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <QVector>

class PooledObject;

class ObjectPool
{
public:
	ObjectPool();
	~ObjectPool();

	QVector<PooledObject*> list;
	void clear();
	void storeInPool(PooledObject*);
};


#endif // OBJECTPOOL_H
