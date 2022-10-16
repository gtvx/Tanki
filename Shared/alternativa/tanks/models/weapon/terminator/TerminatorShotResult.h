#ifndef TERMINATORSHOTRESULT_H
#define TERMINATORSHOTRESULT_H

#include "alternativa/math/vector3.h"
#include "listvector3.h"
#include <QVector>

class TankData;

class TerminatorShotResult
{
public:
	QVector<TankData*> targets;
	ListVector3 hitPoints;
	Vector3 dir;
};

#endif // TERMINATORSHOTRESULT_H
