#ifndef DOMPOINT_H
#define DOMPOINT_H

#include "alternativa/math/vector3.h"
#include "dompointid.h"
#include "_global.h"
#include <memory>

class Object3D;
class DOMModel;


class DomPoint
{
public:
	DOMModel *model;
	std::shared_ptr<Object3D> pedestal;


	std::shared_ptr<Object3D> pedestal_red;
	std::shared_ptr<Object3D> pedestal_blue;
	std::shared_ptr<Object3D> pedestal_none;

	Vector3 pos;
	double radius;
	double serverProgress;
	double clientProgress;
	double progressSpeed;

	DomPointID id;


	//bool updateForced;
	bool capture;

	DomPoint(const DomPointID &id, const Vector3 *pos, double radius, DOMModel *model);
	~DomPoint();
	void tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff);
	void readPos(Vector3 *v);
};

#endif // DOMPOINT_H
