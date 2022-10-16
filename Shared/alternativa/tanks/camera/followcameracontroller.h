#ifndef FOLLOWCAMERACONTROLLER_H
#define FOLLOWCAMERACONTROLLER_H

#include "IFollowCameraController.h"
#include "cameracontrollerbase.h"
#include "CameraPositionData.h"

class ICollisionDetector;


class FollowCameraController : public CameraControllerBase, public IFollowCameraController
{
	ICollisionDetector *collisionDetector;
	int cameraCollisionGroup;
	double _cameraHeight;
	Tank *_tank;
	Vector3 targetPosition;
	Vector3 targetDirection;
	Vector3 position;
	Vector3 rotation;
	int lastMinDistanceTime;
	int noPitchCorrection;
	CameraPositionData cameraPositionData;
	bool active;
	bool keyUpPressed;
	bool keyDownPressed;
	bool locked;
	double linearSpeed;
	double pitchSpeed;
	double yawSpeed;
	double baseElevation;
	double cameraDistance;
	double lastCollisionDistance;
public:
	FollowCameraController(ICollisionDetector *collisionDetector, GameCamera *camera, int cameraCollisionGroup);
	void setDefaultSettings();
	void setAlternateSettings();
	void addModifier(ICameraStateModifier *modifier);
	Tank* getTank();
	void setTank(Tank *value);
	void initByTarget(Vector3 *targetPosition, Vector3 *targetDirection);
	void initCameraComponents();
	void activate();
	void deactivate();
	void update(uint32_t time, uint32_t timeDelta);
	void setLocked(bool locked);
	double getCameraHeight();
	void setCameraHeight(double value);
	void getCameraState(Vector3 *targetPosition, Vector3 *targetDirection, Vector3 *resultingPosition, Vector3 *resultingAngles);
	void onKey(int key, bool down);

private:
	void getCameraPositionData(Vector3 *targetPosition, Vector3 *targetDirection, bool useReboundDelay, double dt, CameraPositionData *result);
	double getCollisionDistance(const Vector3 *rayOrigin, const Vector3 *rayDirection, double rayLength, double minCollisionDistance);
	void updateTargetState();
	void updateHeight();
	double getCamDistance(double h);
	double getLinearSpeed(double positionError);
	double getAngularSpeed(double angleError, double currentSpeed);
	double snap(double value, double snapValue, double epsilon);
	double getPitchAngle(CameraPositionData *cameraPositionData);
	double moveValueTowards(double value, double targetValue, double delta);
};

#endif // FOLLOWCAMERACONTROLLER_H
