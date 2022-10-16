#ifndef BLINKER_H
#define BLINKER_H


class Blinker
{
	int initialInterval;
	int minInterval;
	int intervalDecrement;
	int switchTime;
	int currInterval;

	double maxValue;
	double minValue;
	double speedCoeff;
	double value;
	double speed;
	double valueDelta;

public:
	Blinker(int initialInterval, int minInterval, int intervalDecrement, double minValue, double maxValue, double speedCoeff);

	void init(int now);
	void setMaxValue(double value);
	void setMinValue(double value);
	double updateValue(int now, int delta);
	double getSpeed(double direction);
};

#endif // BLINKER_H
