#ifndef BONUSCACHE_H
#define BONUSCACHE_H

class QString;
class Parachute;
class BonusMesh;
class Parachute2;

#include "__bonus.h"
#include <memory>

#if __BONUS_TYPE == 1
class Cords1;
typedef Cords1 Cords;
#endif

#if __BONUS_TYPE == 2
class Cords2;
typedef Cords2 Cords;
#endif


class BonusCache
{
public:
	static bool isParachuteCacheEmpty();
	static std::shared_ptr<Parachute2> getParachute();
	static void putParachute(std::shared_ptr<Parachute2> parachute);

	static bool isCordsCacheEmpty();
	static std::shared_ptr<Cords> getCords();
	static void putCords(std::shared_ptr<Cords> cords);

	static bool isBonusMeshCacheEmpty(const QString &objectId);
	static std::shared_ptr<BonusMesh> getBonusMesh(const QString &objectId);
	static void putBonusMesh(std::shared_ptr<BonusMesh> bonusMesh);

	static void destroy();
};

#endif // BONUSCACHE_H
