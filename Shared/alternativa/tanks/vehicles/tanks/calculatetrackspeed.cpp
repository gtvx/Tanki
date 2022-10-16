#include "calculatetrackspeed.h"
#include "mymath.h"


static const int FORWARD = 1;
static const int BACK = 2;
static const int LEFT = 4;
static const int RIGHT = 8;
static const double minSpeed = 0.1;
static const double minASpeed = 0.01;


CalculateTrackSpeed::CalculateTrackSpeed()
{
	k_V = 0.01;
	k_aV = 2.49;
	speedSmoother = 0.3;
	antiSSFConst = 1.5;
	trackVelocities[0] = 0;
	trackVelocities[1] = 0;
	trackVelocities[2] = 0;
}

double CalculateTrackSpeed::calcTrackSpeed(double moduleV,
										   double moduleaV,
										   int controlbits,
										   int numContacts,
										   int numCollision,
										   double maxSpeed,
										   int trackID)
{
	int linearMovementDirection = ((((!(controlbits & FORWARD)) && (!(controlbits & BACK))) || ((controlbits & FORWARD) && (controlbits & BACK))) ? 0 : (((!(controlbits & FORWARD)) && (controlbits & BACK)) ? -1 : 1));
	int angularMovementDirection = ((((!(controlbits & LEFT)) && (!(controlbits & RIGHT))) || ((controlbits & LEFT) && (controlbits & RIGHT))) ? 0 : (((!(controlbits & LEFT)) && (controlbits & RIGHT)) ? -1 : 1));
	int tankState = ((numContacts == 0) ? -1 : ((numCollision == 0) ? 0 : 1));
	bool isFreezed = (((moduleV < (minSpeed * 50)) && (moduleaV < (minASpeed * 500))) ? true : false);
	double trackSpeed = (((!(linearMovementDirection == 0)) && (angularMovementDirection == 0)) ? (((linearMovementDirection * this->k_V) * moduleV)) : (((linearMovementDirection == 0) && (!(angularMovementDirection == 0))) ? ((((trackID * angularMovementDirection) * this->k_aV) * moduleaV)) : (((!(linearMovementDirection == 0)) && (angularMovementDirection < 0)) ? ((((MyMath::pow((3. / 4.), (linearMovementDirection * trackID)) * linearMovementDirection) * this->k_V) * moduleV)) : (((!(linearMovementDirection == 0)) && (angularMovementDirection > 0)) ? ((((MyMath::pow((4. / 3.), (linearMovementDirection * trackID)) * linearMovementDirection) * this->k_V) * moduleV)) : ((this->trackVelocities[(trackID + 1)] * (1 - this->speedSmoother)))))));
	if (((!(tankState == 0)) && ((!(linearMovementDirection == 0)) || (!(angularMovementDirection == 0)))))
	{
		double desiredSpeedKoef = (((!(linearMovementDirection == 0)) && (angularMovementDirection == 0)) ? ((linearMovementDirection * minSpeed)) : (((linearMovementDirection == 0) && (!(angularMovementDirection == 0))) ? ((((trackID * angularMovementDirection) * minASpeed) * 10)) : (((!(linearMovementDirection == 0)) && (angularMovementDirection < 0)) ? (((MyMath::pow((3. / 4.), (linearMovementDirection * trackID)) * linearMovementDirection) * minSpeed)) : (((!(linearMovementDirection == 0)) && (angularMovementDirection > 0)) ? (((MyMath::pow((4. / 3.), (linearMovementDirection * trackID)) * linearMovementDirection) * minSpeed)) : ((this->trackVelocities[(trackID + 1)] * (1 - this->speedSmoother)))))));
		trackSpeed = (desiredSpeedKoef * ((tankState == -1) ? ((5 * this->k_V) * maxSpeed) : ((isFreezed == true) ? ((5 * this->k_V) * maxSpeed) : ((10 * this->k_V) * maxSpeed))));
	}
	double actualSpeed = ((MyMath::isNaN(trackSpeed)) ? 0 : trackSpeed);
	if (((MyMath::abs((MyMath::abs(actualSpeed) - 9)) < 0.3) || (MyMath::abs((MyMath::abs(actualSpeed) - 13.5)) < 0.3)))
	{
		actualSpeed = (this->antiSSFConst * actualSpeed);
	}
	this->trackVelocities[(trackID + 1)] = actualSpeed;

	return actualSpeed;
}
