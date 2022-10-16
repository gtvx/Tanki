#include "gamecamera.h"
#include "alternativa/math/vector3.h"
#include "mymath.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/tanks/camera/camerafovcalculator.h"
#include "alternativa/engine3d/core/view.h"

//private static const m:Matrix3 = new Matrix3();

const Object3D::T *GameCamera::TYPE = new Object3D::T("GameCamera");


GameCamera::GameCamera(const TT &t) :
	Camera3D(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	pos.reset();
	xAxis.reset();
	yAxis.reset();
	zAxis.reset();
	//diagramVerticalMargin = 35;
}

void GameCamera::calculateAdditionalData()
{
	double cosX = MyMath::cos(rotation.x);
	double sinX = MyMath::sin(rotation.x);
	double cosY = MyMath::cos(rotation.y);
	double sinY = MyMath::sin(rotation.y);
	double cosZ = MyMath::cos(rotation.z);
	double sinZ = MyMath::sin(rotation.z);
	double cosZsinY = (cosZ * sinY);
	double sinZsinY = (sinZ * sinY);
	this->xAxis.x = (cosZ * cosY);
	this->yAxis.x = ((cosZsinY * sinX) - (sinZ * cosX));
	this->zAxis.x = ((cosZsinY * cosX) + (sinZ * sinX));
	this->xAxis.y = (sinZ * cosY);
	this->yAxis.y = ((sinZsinY * sinX) + (cosZ * cosX));
	this->zAxis.y = ((sinZsinY * cosX) - (cosZ * sinX));
	this->xAxis.z = -(sinY);
	this->yAxis.z = (cosY * sinX);
	this->zAxis.z = (cosY * cosX);
	this->pos.x = position.x;
	this->pos.y = position.y;
	this->pos.z = position.z;
}

void GameCamera::getGlobalVector(const Vector3 *param1, Vector3 *param2)
{
	Matrix3 m;
	m.setRotationMatrix(rotation.x, rotation.y, rotation.z);
	m.transformVector(param1, param2);
}

void GameCamera::updateFov()
{
	fov = CameraFovCalculator::getCameraFov(view->getWidth(), view->getHeight());
}

void GameCamera::clonePropertiesFromGameCamera(GameCamera *src)
{
	clonePropertiesFromCamera3D(src);
}


std::shared_ptr<GameCamera> new_GameCamera()
{
	return std::make_shared<GameCamera>(Object3D::TT(GameCamera::TYPE));
}


std::shared_ptr<GameCamera> GameCamera_clone(const std::shared_ptr<GameCamera> &in)
{
	if (in->isType(GameCamera::TYPE)) {
		auto out = new_GameCamera();
		out->clonePropertiesFromGameCamera(in.get());
		return out;
	}

	throw 41132423;
}
