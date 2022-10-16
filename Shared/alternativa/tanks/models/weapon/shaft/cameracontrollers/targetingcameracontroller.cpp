#include "targetingcameracontroller.h"
#include "alternativa/math/vector3.h"
#include "alternativa/math/matrix3.h"
#include "mymath.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "randomgenerator.h"
#include "service.h"

//private static const objectMatrix:Matrix3 = new Matrix3();
//private static const cameraMatrix:Matrix3 = new Matrix3();
//private static const vector:Vector3 = new Vector3();

//this.shaft_freq = new ConsoleVarFloat("shf", 0.05, -100, 100);
//this.shaft_vel = new ConsoleVarFloat("shv", 0.05, -100, 100);
//this.shaft_avel = new ConsoleVarFloat("shav", 0.2, -100, 100);

static const double shaft_freq = 0.05;
static const double shaft_vel = 0.05;
static const double shaft_avel = 0.2;



TargetingCameraController::TargetingCameraController(double maxElevation, double minElevation, double maxElevationSpeed, double elevationAcceleration)
{
	this->localPosition = new Vector3();
	this->direction = new Vector3();

	this->_elevation = 0;
	this->_elevationDirection = 0;
	this->elevationSpeed = 0;
	this->maxElevationSpeed = 1;
	this->maxElevationSpeedFactor = 1;
	this->elevationSpeed = 0;
	this->elevationAcceleration = 5;
	this->sign = 1;
	this->modX = 0;
	this->modZ = 0;
	this->ind = true;
	this->maxTime = 400000;
	this->maxRadius = 5;
	this->maxAngle = 3.14159265358979;
	this->radius = 0;

	this->maxElevation = maxElevation;
	this->minElevation = minElevation;
	this->maxElevationSpeed = maxElevationSpeed;
	this->elevationAcceleration = elevationAcceleration;
	this->reload();
	this->battlefieldModel = Service::getBattlefieldModel();

	this->direction->reset((1 - (2 * RandomGenerator::random())), (1 - (2 * RandomGenerator::random())), (1 - (2 * RandomGenerator::random())));
	this->direction->normalize();

	this->cameraPosition = new Vector3();
	this->ang = (RandomGenerator::random() * 2) * MyMath::PI;

}

TargetingCameraController::~TargetingCameraController()
{

}

void TargetingCameraController::reload()
{
	this->modX = 0;
	this->modZ = 0;
	this->sign = 1;
}

void TargetingCameraController::setElevationDirection(int value)
{
	if (this->_elevationDirection != value)
	{
		this->_elevationDirection = value;
		this->elevationSpeed = 0;
	}
}

bool TargetingCameraController::isMaxElevation()
{
	return (this->_elevation == this->maxElevation) || (this->_elevation == this->minElevation);
}



void TargetingCameraController::readCameraPosition(Vector3 *out)
{
	out->copy(&this->battlefieldModel->bfData->viewport->camera->position);
}

void TargetingCameraController::readCameraDirection(Vector3 *out)
{
	out->copy(&this->battlefieldModel->bfData->viewport->camera->zAxis);
}

double TargetingCameraController::getElevation()
{
	return this->_elevation;
}

void TargetingCameraController::setElevation(double value)
{
	if (value > this->maxElevation)
	{
		this->_elevation = this->maxElevation;
	}
	else
	{
		if (value < this->minElevation)
		{
			this->_elevation = this->minElevation;
		}
		else
		{
			this->_elevation = value;
		}
	}
}

void TargetingCameraController::setAnchorObject(Object3D *object)
{
	this->anchorObject = object;
}

void TargetingCameraController::setLocalPosition(const Vector3 *pos)
{
	this->localPosition->copy(pos);
}

void TargetingCameraController::setMaxElevationSpeedFactor(double value)
{
	this->maxElevationSpeedFactor = value;
}

void TargetingCameraController::update(uint32_t param1, uint32_t param2)
{
	(void)param1;

	double _loc3_ = (double)(int32_t)param2 / 1000.;
	if (this->_elevationDirection != 0)
	{
		this->elevationSpeed = (this->elevationSpeed + (this->elevationAcceleration * _loc3_));
		double _loc5_ = (this->maxElevationSpeed * this->maxElevationSpeedFactor);
		if (this->elevationSpeed > _loc5_)
		{
			this->elevationSpeed = _loc5_;
		}
		this->_elevation = this->_elevation + ((this->_elevationDirection * this->elevationSpeed) * _loc3_);
		if (this->_elevation > this->maxElevation)
		{
			this->_elevation = this->maxElevation;
		}
		else
		{
			if (this->_elevation < this->minElevation)
			{
				this->_elevation = this->minElevation;
			}
		}
	}

	double freequency = shaft_freq;
	double vel = shaft_vel;
	double avel = shaft_avel;

	Matrix3 objectMatrix;
	Matrix3 cameraMatrix;
	Vector3 vector;

	objectMatrix.setRotationMatrix(this->anchorObject->rotation.x, this->anchorObject->rotation.y, this->anchorObject->rotation.z);
	cameraMatrix.fromAxisAngle(&Vector3::X_AXIS, (this->_elevation - (MyMath::PI / 2)));
	cameraMatrix.append(&objectMatrix);
	cameraMatrix.getEulerAngles(&vector);
	GameCamera *cam = this->battlefieldModel->bfData->viewport->camera;
	Vector3 delta;
	this->getDeltas(vel, avel, freequency, 100, &delta);
	bool isMaxElevation = (this->_elevation == this->maxElevation);
	//bool isMinElevation = (this->_elevation == this->minElevation);
	this->modX = (this->modX + ((!(!(isMaxElevation))) ? 0 : delta.x));
	this->modZ = (this->modZ + delta.z);
	cam->rotation.x = vector.x + this->modX;
	cam->rotation.y = vector.y;
	cam->rotation.z = vector.z + this->modZ;
	objectMatrix.transformVector(this->localPosition, &vector);
	cam->position.x = vector.x + this->anchorObject->position.x;
	cam->position.y = vector.y + this->anchorObject->position.y;
	cam->position.z = vector.z + this->anchorObject->position.z;
}

void TargetingCameraController::setCameraFov(double value)
{
	this->battlefieldModel->bfData->viewport->camera->fov = value;
}

double TargetingCameraController::getRandom()
{
	double rd1 = RandomGenerator::random();
	double rd2 = RandomGenerator::random();
	return (rd1 > 0.5) ? -rd2 : rd2;
}

double TargetingCameraController::stableRandom(double k)
{
	double rd1 = RandomGenerator::random();
	double rd2 = RandomGenerator::random();
	this->sign = (rd2 > MyMath::pow(0.5, k)) ? -this->sign : this->sign;
	return this->sign * rd1;
}

double TargetingCameraController::stableRandomSign(double k)
{
	this->sign = (RandomGenerator::random() > MyMath::pow(0.5, k)) ? -this->sign : this->sign;
	return this->sign;
}

void TargetingCameraController::getDeltas(double vel, double avel, double freeq, double dist, Vector3 *result)
{
	this->ang = this->ang + (this->stableRandom(freeq) * avel);
	result->x = MyMath::atan2(((vel * MyMath::cos(this->ang)) * RandomGenerator::random()), dist);
	result->y = 0;
	result->z = MyMath::atan2(((vel * MyMath::sin(this->ang)) * RandomGenerator::random()), dist);
}
