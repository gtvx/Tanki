#include "mathutils.h"
#include "mymath.h"


double MathUtils::clampAngle(double radians)
{
	radians = MyMath::fmod(radians, MyMath::PI2);
	if (radians < -MyMath::PI)
		return MyMath::PI2 + radians;
	if (radians > MyMath::PI)
		return radians - MyMath::PI2;
	return radians;
}

double MathUtils::clampAngleFast(double radians)
{
	if (radians < -MyMath::PI)
		return MyMath::PI2 + radians;
	if (radians > MyMath::PI)
		return radians - MyMath::PI2;
	return radians;
}

double MathUtils::clamp(double value, double min, double max)
{
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}
