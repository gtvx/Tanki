#include "camerafovcalculator.h"
#include "mymath.h"


#define NARROW_SCREEN 12.
#define WIDE_SCREEN 16.
#define DIVIDER 9.
#define DEFAULT_FOV (MyMath::PI / 2.) //1.5707963267949


double CameraFovCalculator::getCameraFov(double param1, double param2)
{
	double _loc3_ = (param2 / DIVIDER);
	double _loc4_ = (param1 / _loc3_);
	if (_loc4_ <= NARROW_SCREEN)
	{
		return DEFAULT_FOV;
	}
	double _loc5_ = (_loc4_ - (WIDE_SCREEN - NARROW_SCREEN));
	if (_loc5_ < NARROW_SCREEN)
	{
		_loc5_ = NARROW_SCREEN;
	}
	double _loc6_ = _loc5_ * _loc3_;
	double _loc7_ = (MyMath::sqrt(((_loc6_ * _loc6_) + (param2 * param2))) * 0.5) / MyMath::tan((DEFAULT_FOV * 0.5));
	return MyMath::atan(((MyMath::sqrt(((param1 * param1) + (param2 * param2))) * 0.5) / _loc7_)) * 2.;
}
