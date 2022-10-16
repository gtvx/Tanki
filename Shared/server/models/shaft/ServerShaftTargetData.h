#ifndef SERVERSHAFTTARGETDATA_H
#define SERVERSHAFTTARGETDATA_H

#include "alternativa/math/vector3.h"
#include <QString>

class ServerShaftTargetData
{
public:
	Vector3 hitPoint;
	Vector3 globalHitPoint;
	QString targetId;

	ServerShaftTargetData(const Vector3 *hitPoint, const Vector3 *globalHitPoint, const QString &targetId)
	{
		this->hitPoint.copy(hitPoint);
		this->globalHitPoint.copy(globalHitPoint);
		this->targetId = targetId;
	}
};

#endif // SERVERSHAFTTARGETDATA_H
