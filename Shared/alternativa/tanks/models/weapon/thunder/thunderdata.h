#ifndef THUNDERDATA_H
#define THUNDERDATA_H

class ThunderData
{
public:
	double maxSplashDamageRadius;
	double minSplashDamageRadius;
	double minSplashDamagePercent;
	double impactForce;

	ThunderData(double maxSplashDamageRadius,
				double minSplashDamageRadius,
				double minSplashDamagePercent,
				double impactForce);
};

#endif // THUNDERDATA_H
