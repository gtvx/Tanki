#include "cachedentitystat.h"

CachedEntityStat::CachedEntityStat()
{
	this->clear();
}

void CachedEntityStat::clear()
{
	this->requestCount = 0;
	this->releaseCount = 0;
	this->createCount = 0;
	this->destroyCount = 0;
}
