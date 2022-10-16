#ifndef LOCALHULLTRANSFORMUPDATER_H
#define LOCALHULLTRANSFORMUPDATER_H

#include "HullTransformUpdater.h"

class Tank;

class LocalHullTransformUpdater : public HullTransformUpdater
{
	Tank *tank;
public:
	LocalHullTransformUpdater(Tank *tank);
	~LocalHullTransformUpdater();

	void reset();
	void update(double);
};

#endif // LOCALHULLTRANSFORMUPDATER_H
