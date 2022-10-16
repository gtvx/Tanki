#ifndef SHAFTSHOTRESULT_H
#define SHAFTSHOTRESULT_H

#include "alternativa/math/vector3.h"
#include "listvector3.h"
#include <QVector>

class TankData;

class ShaftShotResult
{
public:
	QVector<TankData*> targets;
	ListVector3 hitPoints;

	Vector3 dir;

	QString toJSON();
};

#endif // SHAFTSHOTRESULT_H
