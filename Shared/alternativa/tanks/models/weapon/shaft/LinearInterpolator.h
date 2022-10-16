#ifndef LINEARINTERPOLATOR_H
#define LINEARINTERPOLATOR_H

class LinearInterpolator
{
	double a;
	double b;
public:

	LinearInterpolator(double a = 0, double b = 1)
	{
		this->a = a;
		this->b = b;
	}

	void setInterval(double a, double b)
	{
		this->a = a;
		this->b = b;
	}

	double interpolate(double v)
	{
		return this->a + ((this->b - this->a) * v);
	}
};

#endif // LINEARINTERPOLATOR_H
