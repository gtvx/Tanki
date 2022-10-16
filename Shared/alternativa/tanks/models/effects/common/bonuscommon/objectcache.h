#ifndef OBJECTCACHE_H
#define OBJECTCACHE_H

#include <qglobal.h>

class ObjectCache
{
	int size;
	QVector<void*> *objects;

public:
	ObjectCache();
	~ObjectCache();

	void put(void *object);
	void* get();
	bool isEmpty();
};

#endif // OBJECTCACHE_H
