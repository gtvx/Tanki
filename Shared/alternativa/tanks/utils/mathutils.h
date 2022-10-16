#ifndef MATHUTILS_H
#define MATHUTILS_H


class MathUtils
{
public:
	static double clampAngle(double radians);
	static double clampAngleFast(double radians);
	static double clamp(double value, double min, double max);

	static int getBitValue(int v1, int v2)
	{
		return (v1 >> v2) & 0x01;
	}
};

#endif // MATHUTILS_H
