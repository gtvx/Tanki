#ifndef RAILGUNSHOTRESULT_H
#define RAILGUNSHOTRESULT_H

#include "alternativa/math/vector3.h"
#include "listvector3.h"
#include <QVector>
#include <QString>

class TankData;

class RailgunShotResult
{
public:
	QVector<TankData*> targets;
	ListVector3 hitPoints;
	Vector3 dir;
};

#endif // RAILGUNSHOTRESULT_H
