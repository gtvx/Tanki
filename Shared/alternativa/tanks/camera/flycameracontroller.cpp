#include "flycameracontroller.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/utils/mathutils.h"
#include "anglevalues.h"


static const double FLY_HEIGHT = 3000;


FlyCameraController::FlyCameraController(GameCamera *camera) :
	CameraControllerBase(camera)
{
	angleValuesX = new AngleValues;
	angleValuesZ = new AngleValues;
}

FlyCameraController::~FlyCameraController()
{
	delete angleValuesX;
	delete angleValuesZ;
}

void FlyCameraController::init(const Vector3 *targetPosition, const Vector3 *targetAngles, int travelTime)
{
	this->p1.copy(&camera->pos);
	this->p2.copy(&this->p1);
	this->p4.copy(targetPosition);
	this->p3.copy(&this->p4);
	this->p2.z = this->p3.z = (((this->p1.z > this->p4.z) ? this->p1.z : this->p4.z) + FLY_HEIGHT);
	double k = double(0x3D0900) / double(travelTime * travelTime);
	this->angleValuesX->init(MathUtils::clampAngle(camera->rotation.x), targetAngles->x, k);
	this->angleValuesZ->init(MathUtils::clampAngle(camera->rotation.z), targetAngles->z, k);

	Vector3 v;
	v.diff(&this->p4, &this->p1);
	this->totalDistance = v.length();
	this->acceleration = this->totalDistance * k;
	this->distance = 0;
	this->speed = 0;
}

void FlyCameraController::update(uint32_t time, uint32_t deltaMsec)
{
	(void)time;

	if (this->speed < 0)
	{
		return;
	}

	if (((this->distance > (0.5 * this->totalDistance)) && (this->acceleration > 0)))
	{
		this->acceleration = -(this->acceleration);
		this->angleValuesX->reverseAcceleration();
		this->angleValuesZ->reverseAcceleration();
	}
	double dt = 0.001 * deltaMsec;
	double dv = this->acceleration * dt;
	this->distance = (this->distance + ((this->speed + (0.5 * dv)) * dt));
	this->speed = this->speed + dv;
	if (this->distance > this->totalDistance)
	{
		this->distance = this->totalDistance;
	}
	Vector3 cameraPosition;
	this->bezier((this->distance / this->totalDistance), &this->p1, &this->p2, &this->p3, &this->p4, &cameraPosition);
	setPosition(&cameraPosition);
	rotateBy(this->angleValuesX->update(dt), 0, this->angleValuesZ->update(dt));
}

void FlyCameraController::bezier(double t, Vector3 *p1, Vector3 *p2, Vector3 *p3, Vector3 *p4, Vector3 *result)
{
	double  t1 = 1 - t;
	double a = t1 * t1;
	double b = (3 * t) * a;
	a = a * t1;
	double d = t * t;
	double c = (3 * d) * t1;
	d = d * t;
	result->x = (a * p1->x) + (b * p2->x) + (c * p3->x) + (d * p4->x);
	result->y = (a * p1->y) + (b * p2->y) + (c * p3->y) + (d * p4->y);
	result->z = (a * p1->z) + (b * p2->z) + (c * p3->z) + (d * p4->z);
}
