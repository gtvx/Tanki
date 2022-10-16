#ifndef LIGHTDATAMANAGER_H
#define LIGHTDATAMANAGER_H

#include <stdint.h>

class QByteArray;
class QString;
class LightAnimation;


class LightDataManager
{
public:
	static void init(const QByteArray &json_);
	static LightAnimation* getLightDataMuzzle(const QString &turretId);
	static LightAnimation* getLightDataShot(const QString &turretId);
	static LightAnimation* getLightDataExplosion(const QString &turretId);
	static LightAnimation* getLightData(const QString &turretId, const QString &effectName);
	static uint32_t getBonusLightColor(const QString &bonusId);
};

#endif // LIGHTDATAMANAGER_H
