#ifndef ANGLEVALUES_H
#define ANGLEVALUES_H


class AngleValues
{
	double currentAngle;
	double totalAngle;
	double angularAcceleration;
	double angularSpeed;
	double angleDirection;

public:
	AngleValues();

	void init(double  startAngle, double targetAngle, double accelerationCoeff);
	void reverseAcceleration();
	double update(double dt);
};

#endif // ANGLEVALUES_H
