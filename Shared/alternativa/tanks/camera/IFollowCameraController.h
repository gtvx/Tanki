#ifndef IFOLLOWCAMERACONTROLLER_H
#define IFOLLOWCAMERACONTROLLER_H

#include "ICameraController.h"

class Vector3;
class Tank;
class ICameraStateModifier;

class IFollowCameraController : public ICameraController
{
public:
	virtual double getCameraHeight() = 0;
	virtual void setCameraHeight(double) = 0;
	virtual void deactivate() = 0;
	virtual void activate() = 0;
	virtual void setLocked(bool) = 0;
	virtual void initByTarget(Vector3*, Vector3*) = 0;
	virtual void setTank(Tank*) = 0;
	virtual Tank* getTank() = 0;
	virtual void addModifier(ICameraStateModifier*) = 0;
	virtual void initCameraComponents() = 0;
	virtual void getCameraState(Vector3*, Vector3*, Vector3*, Vector3*) = 0;
	virtual void onKey(int key, bool down) = 0;
	virtual ~IFollowCameraController() {}
};

#endif // IFOLLOWCAMERACONTROLLER_H
