#include "bonuscache.h"
#include <QHash>
#include "bonusmesh.h"
#include "objectcache.h"
#include <QStack>


static QHash<QString, QStack<std::shared_ptr<BonusMesh>>*> boxCaches;
static QStack<std::shared_ptr<Cords>> cordsCache;
static QStack<std::shared_ptr<Parachute2>> parachuteCache;


/*
static ObjectCache* getBonusMeshCache(const QString &objectId)
{
	ObjectCache *cache = boxCaches.value(objectId);

	if (cache == nullptr)
	{
		cache = new ObjectCache();
		boxCaches.insert(objectId, cache);
	}

	return cache;
}
*/

static QStack<std::shared_ptr<BonusMesh>>* getBonusMeshCache(const QString &objectId)
{
	QStack<std::shared_ptr<BonusMesh>> *cache = boxCaches.value(objectId);

	if (cache == nullptr)
	{
		cache = new QStack<std::shared_ptr<BonusMesh>>();
		boxCaches.insert(objectId, cache);
	}

	return cache;
}




bool BonusCache::isParachuteCacheEmpty()
{
	return parachuteCache.isEmpty();
}

std::shared_ptr<Parachute2> BonusCache::getParachute()
{
	return parachuteCache.pop();
}

void BonusCache::putParachute(std::shared_ptr<Parachute2> parachute)
{
	parachuteCache.append(parachute);
}




bool BonusCache::isCordsCacheEmpty()
{
	return cordsCache.isEmpty();
}

std::shared_ptr<Cords> BonusCache::getCords()
{
	return cordsCache.pop();
}

void BonusCache::putCords(std::shared_ptr<Cords> cords)
{
	cordsCache.append(cords);
}




bool BonusCache::isBonusMeshCacheEmpty(const QString &objectId)
{
	return getBonusMeshCache(objectId.split("_")[0])->isEmpty();
}

std::shared_ptr<BonusMesh> BonusCache::getBonusMesh(const QString &objectId)
{
	return getBonusMeshCache(objectId.split("_")[0])->pop();
}

void BonusCache::putBonusMesh(std::shared_ptr<BonusMesh> bonusMesh)
{
	getBonusMeshCache(bonusMesh->getObjectId().split("_")[0])->append(bonusMesh);
}





void BonusCache::destroy()
{
	cordsCache.clear();
	parachuteCache.clear();

	QHashIterator<QString, QStack<std::shared_ptr<BonusMesh>>*> i(boxCaches);
	while (i.hasNext()) {
		i.next();
		delete i.value();
	}
}
