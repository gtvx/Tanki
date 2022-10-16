#ifndef DOMPOINTDATA_H
#define DOMPOINTDATA_H

#include "alternativa/math/vector3.h"
#include "../dompointid.h"
#include <QStringList>

class DOMPointData
{
public:
	Vector3 pos;
	double radius;
	double score;
	DomPointID id;
	QStringList occupatedUsers;
};

#endif // DOMPOINTDATA_H
