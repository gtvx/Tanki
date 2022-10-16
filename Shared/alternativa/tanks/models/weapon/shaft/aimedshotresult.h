#ifndef AIMEDSHOTRESULT_H
#define AIMEDSHOTRESULT_H

#include "alternativa/math/vector3.h"
#include <QVector>

class Tank;
class Aim;

class AimedShotResult
{
public:
	QVector<Aim*> aims;

	bool isStatic;

	Vector3 staticHitPoint;
	Vector3 targetHitPoint;

	AimedShotResult();


	void setTarget(Tank *tank, const Vector3 *param2);
	void setStaticHitPoint(const Vector3 *staticHitPoint);
};

#endif // AIMEDSHOTRESULT_H
