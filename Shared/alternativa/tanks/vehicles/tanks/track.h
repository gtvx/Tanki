#ifndef TRACK_H
#define TRACK_H

#include "alternativa/math/vector3.h"
#include <stdint.h>

class Tank;
class SuspensionRay;
class SuspensionData;


class Track
{
public:
	Vector3 averageSurfaceVelocity;
	Tank *tank;
	SuspensionRay *rays[10];
	uint32_t raysNum;
	uint32_t lastContactsNum;
	double bot_turn_add_value;

	Track(Tank *tank, int raysNum, Vector3 *relPos, double trackLength);
	~Track();
	void setTrackParams(int raysNum, Vector3 *relPos, double trackLength);
	void setCollisionGroup(int value);
	void addForces(double dt, double throttle, double maxSpeed, int slipTerm, double weight, SuspensionData *data, bool brake);
};

#endif // TRACK_H
