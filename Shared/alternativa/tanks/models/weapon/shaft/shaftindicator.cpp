#include "shaftindicator.h"
#include "alternativa/object/clientobjectweapon.h"
#include "resource/resourceutil.h"
#include <QString>


BitmapData* ShaftIndicator::getIndicator(ClientObjectWeapon *turret)
{
	QString m;
	QString id = turret->getId();

	if (id.endsWith("_m0")) {
		m = "0";
	} else if (id.endsWith("_m1")) {
		m = "1";
	} else if (id.endsWith("_m2")) {
		m = "2";
	} else if (id.endsWith("_m3")) {
		m = "3";
	} else {
		m = "0";
	}

	return ResourceUtil::getResourceImageLocal("shaft/Indicator_aim_m" + m + ".png").get();
}
