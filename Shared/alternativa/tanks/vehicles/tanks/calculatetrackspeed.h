#ifndef CALCULATETRACKSPEED_H
#define CALCULATETRACKSPEED_H


class CalculateTrackSpeed
{
	double k_V;
	double k_aV;
	double speedSmoother;
	double antiSSFConst;
	double trackVelocities[3];

public:
	CalculateTrackSpeed();

	double calcTrackSpeed(double moduleV, double moduleaV, int controlbits, int numContacts, int numCollision, double maxSpeed, int trackID);
};

#endif // CALCULATETRACKSPEED_H
