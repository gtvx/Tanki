#ifndef CAMERACONTROLLERBASE_H
#define CAMERACONTROLLERBASE_H

class GameCamera;
class Vector3;

class CameraControllerBase
{
public:
	CameraControllerBase(GameCamera *camera);
	GameCamera* getCamera();

protected:
	GameCamera *camera;

	void setPosition(Vector3 *position);
	void setOrientation(Vector3 *eulerAngles);
	void setOrientationXYZ(double  rx, double  ry, double rz);
	void moveBy(double dx, double dy, double dz);
	void rotateBy(double rx, double ry, double rz);
};

#endif // CAMERACONTROLLERBASE_H
