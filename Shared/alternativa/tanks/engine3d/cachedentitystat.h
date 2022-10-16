#ifndef CACHEDENTITYSTAT_H
#define CACHEDENTITYSTAT_H


class CachedEntityStat
{
public:

	int requestCount;
	int releaseCount;
	int createCount;
	int destroyCount;

	CachedEntityStat();

	void clear();
};

#endif // CACHEDENTITYSTAT_H
