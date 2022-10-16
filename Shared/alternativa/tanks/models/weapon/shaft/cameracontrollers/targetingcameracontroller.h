#ifndef TARGETINGCAMERACONTROLLER_H
#define TARGETINGCAMERACONTROLLER_H

#include "alternativa/tanks/camera/ICameraController.h"

class BattlefieldModel;
class Vector3;
class Object3D;


class TargetingCameraController : public ICameraController
{
	double _elevation;
	double maxElevation;
	double minElevation;
	double maxElevationSpeed;
	double maxElevationSpeedFactor;
	double elevationSpeed;
	double elevationAcceleration;
	double modX;
	double modZ;
	double ang;

	int _elevationDirection;
	int sign;

	bool ind;

	BattlefieldModel *battlefieldModel;
	Vector3 *localPosition;
	Vector3 *cameraPosition;
	Object3D *anchorObject;
public:

	Vector3 *direction;
	double maxTime;
	double maxRadius;
	double maxAngle;
	double radius;

	TargetingCameraController(double maxElevation, double minElevation, double maxElevationSpeed, double elevationAcceleration);
	virtual ~TargetingCameraController();

	void reload();
	void setElevationDirection(int value);
	bool isMaxElevation();
	void readCameraPosition(Vector3 *out);
	void readCameraDirection(Vector3 *out);
	double getElevation();
	void setElevation(double value);
	void setAnchorObject(Object3D *object);
	void setLocalPosition(const Vector3 *pos);
	void setMaxElevationSpeedFactor(double value);
	void update(uint32_t param1, uint32_t param2) override;
	void setCameraFov(double value);
	double getRandom();

private:
	double stableRandom(double k);
	double stableRandomSign(double k);
	void getDeltas(double vel, double avel, double freeq, double dist, Vector3 *result);
};

#endif // TARGETINGCAMERACONTROLLER_H
