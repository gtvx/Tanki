#include "lightdatamanager.h"
#include "lightingeffectrecord.h"
#include "lightanimation.h"
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>



static QHash<QString, LightAnimation*> data;
static QHash<QString, uint32_t> colors;


void LightDataManager::init(const QByteArray &json_)
{
	QJsonDocument doc = QJsonDocument::fromJson(json_);
	QJsonArray items = doc.object().value("data").toArray();

	for (int i = 0; i < items.count(); i++)
	{
		QJsonObject item = items.at(i).toObject();
		QString turret = item.value("turret").toString();

		if (turret.split("_")[0] == "bonus")
		{
			colors.insert(turret, item.value("color").toInt());
		}
		else
		{
			QJsonArray animation = item.value("animation").toArray();

			QString turretId = turret;
			QVector<LightingEffectRecord*> animations;
			for (int i = 0; i < animation.count(); i++)
			{
				QJsonObject anim = animation.at(i).toObject();
				double attenuationBegin = anim.value("attenuationBegin").toDouble();
				double attenuationEnd = anim.value("attenuationEnd").toDouble();
				uint32_t color = anim.value("color").toInt();
				double intensity = anim.value("intensity").toDouble();
				int time = anim.value("time").toInt();
				animations.append(new LightingEffectRecord(attenuationBegin, attenuationEnd, color, intensity, time));
			}
			data.insert(turretId, new LightAnimation(animations));
		}
	}

}

LightAnimation* LightDataManager::getLightDataMuzzle(const QString &turretId)
{
	return (data[(turretId + "_muzzle")]);
}

LightAnimation* LightDataManager::getLightDataShot(const QString &turretId)
{
	return (data[(turretId + "_shot")]);
}

LightAnimation* LightDataManager::getLightDataExplosion(const QString &turretId)
{
	return (data[(turretId + "_explosion")]);
}

LightAnimation* LightDataManager::getLightData(const QString &turretId, const QString &effectName)
{
	return (data[((turretId + "_") + effectName)]);
}

uint32_t LightDataManager::getBonusLightColor(const QString &bonusId)
{
	return (colors[bonusId]);
}
