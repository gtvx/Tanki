#include "thunderdata.h"

ThunderData::ThunderData(double maxSplashDamageRadius, double minSplashDamageRadius, double minSplashDamagePercent, double impactForce)
{
	this->maxSplashDamageRadius = maxSplashDamageRadius;
	this->minSplashDamageRadius = minSplashDamageRadius;
	this->minSplashDamagePercent = minSplashDamagePercent;
	this->impactForce = impactForce;
}
