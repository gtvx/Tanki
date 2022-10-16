#include "cameracontrollerbase.h"
#include "alternativa/math/vector3.h"
#include "gamecamera.h"


CameraControllerBase::CameraControllerBase(GameCamera *camera)
{
	if (camera == nullptr)
		throw 243542;

	this->camera = camera;
}

GameCamera *CameraControllerBase::getCamera()
{
	return this->camera;
}

void CameraControllerBase::setPosition(Vector3 *position)
{
	this->camera->position.x = position->x;
	this->camera->position.y = position->y;
	this->camera->position.z = position->z;
}

void CameraControllerBase::setOrientation(Vector3 *eulerAngles)
{
	this->camera->rotation.copy(eulerAngles);
}

void CameraControllerBase::setOrientationXYZ(double rx, double ry, double rz)
{
	this->camera->rotation.x = rx;
	this->camera->rotation.y = ry;
	this->camera->rotation.z = rz;
}

void CameraControllerBase::moveBy(double dx, double dy, double dz)
{
	this->camera->position.x += dx;
	this->camera->position.y += dy;
	this->camera->position.z += dz;
}

void CameraControllerBase::rotateBy(double rx, double ry, double rz)
{
	this->camera->rotation.x += rx;
	this->camera->rotation.y += ry;
	this->camera->rotation.z += rz;
}
