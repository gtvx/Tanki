#include "objectcache.h"
#include <QVector>


ObjectCache::ObjectCache()
{
	this->objects = new QVector<void*>;
}

ObjectCache::~ObjectCache()
{
	delete this->objects;
}

void ObjectCache::put(void *object)
{
	this->objects->append(object);
}

void* ObjectCache::get()
{
	if (this->size == 0)
		throw 45314;

	this->size--;
	return this->objects->takeLast();
}

bool ObjectCache::isEmpty()
{
	return this->size == 0;
}
