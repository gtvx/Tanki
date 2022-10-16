#ifndef REMOTEHULLTRANSFORMUPDATER_H
#define REMOTEHULLTRANSFORMUPDATER_H


#include "HullTransformUpdater.h"
#include "alternativa/math/quaternion.h"
#include "alternativa/math/vector3.h"

class Tank;

class RemoteHullTransformUpdater : public HullTransformUpdater
{
	Vector3 smoothedPosition;
	Quaternion smoothedOrientation;
	Tank *tank;

public:
	RemoteHullTransformUpdater(Tank *tank);
	void reset();
	void update(double param1);
};

#endif // REMOTEHULLTRANSFORMUPDATER_H
