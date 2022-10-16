#include "followcameracontroller.h"
#include "gamecamera.h"
#include "alternativa/tanks/utils/mathutils.h"
#include "mymath.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "gametime.h"
#include "_global.h"
#include "keys.h"
#include "flash/geom/matrix3d.h"



/*
public static var effectsEnabled:Boolean = true;
public static var maxPositionError:Number = 10;
public static var maxAngleError:Number = (Math.PI / 180);//0.0174532925199433
public static var camSpeedThreshold:Number = 10;
private static var fixedPitch:ConsoleVarFloat = new ConsoleVarFloat("cam_fixedpitch", ((10 * Math.PI) / 180), -1, 1);
private static var noPitchCorrection:ConsoleVarInt = new ConsoleVarInt("cam_nopitchcorrection", 1, 0, 1);
private static var pitchCorrectionCoeff:ConsoleVarFloat = new ConsoleVarFloat("cam_pitchcorrectioncoeff", 1, 0, 10);
private static var reboundDelay:ConsoleVarInt = new ConsoleVarInt("cam_rebound", 1000, 0, 2000);
private static var elevationAngles:Vector.<Number> = new Vector.<Number>(1, true);
private static var rotationMatrix:Matrix3 = new Matrix3();
private static var axis:Vector3 = new Vector3();
private static var rayDirection:Vector3 = new Vector3();
private static const MIN_CAMERA_ANGLE:Number = ((5 * Math.PI) / 180);//0.0872664625997165
private static const KEY_CAMERA_UP:uint = Keyboard.Q;//81
private static const ALT_KEY_CAMERA_UP:uint = Keyboard.PAGE_UP;//33
private static const KEY_CAMERA_DOWN:uint = Keyboard.E;//69
private static const ALT_KEY_CAMERA_DOWN:uint = Keyboard.PAGE_DOWN;//34
private static const HEIGHT_CHANGE_STEP:Number = 50;
private static const MIN_HEIGHT:Number = 150;
private static const MAX_HEIGHT:Number = 3100;
private static const MIN_DISTANCE:Number = 300;
private static const COLLISION_OFFSET:Number = 50;
private static var currentPosition:Vector3 = new Vector3();
private static var currentRotation:Vector3 = new Vector3();
private static var vin:Vector.<Number> = Vector.<Number>([0, 0, 0, 0, 1, 0]);
private static var vout:Vector.<Number> = Vector.<Number>([0, 0, 0, 0, 1, 0]);
private static var rayOrigin:Vector3 = new Vector3();
private static var flatDirection:Vector3 = new Vector3();
private static var positionDelta:Vector3 = new Vector3();
private static var rayIntersection:RayIntersection = new RayIntersection();
*/


//static bool effectsEnabled = true;
static const double maxPositionError = 10;
static const double camSpeedThreshold = 10;
static const double MIN_HEIGHT = 150.;
static const double MAX_HEIGHT = 3100.;
static const double MIN_DISTANCE = 300.;
static const double COLLISION_OFFSET = 50.;
static const double reboundDelay = 1000.;
static const double HEIGHT_CHANGE_STEP = 50; //50
static const double maxAngleError = (MyMath::PI / 180.); //0.0174532925199433
static const double fixedPitch = (10. * MyMath::PI) / 180.;
static const double MIN_CAMERA_ANGLE = (5 * MyMath::PI) / 180; //0.0872664625997165
static const double pitchCorrectionCoeff = 1;



FollowCameraController::FollowCameraController(ICollisionDetector *collisionDetector, GameCamera *camera, int cameraCollisionGroup) :
	CameraControllerBase(camera)
{
	_function_name("FollowCameraController::FollowCameraController");

	if (collisionDetector == nullptr)
		throw 24523423;

	this->noPitchCorrection = 1;
	this->_cameraHeight = 0;
	lastMinDistanceTime = 0;
	linearSpeed = 0;
	pitchSpeed = 0;
	yawSpeed = 0;
	baseElevation = 0;
	cameraDistance = 0;
	lastCollisionDistance = 0;
	targetPosition.reset();
	targetDirection.reset();
	position.reset();
	rotation.reset();
	cameraPositionData.position.reset();
	cameraPositionData.t = 0;
	cameraPositionData.extraPitch = 0;


	this->collisionDetector = collisionDetector;
	this->cameraCollisionGroup = cameraCollisionGroup;
	this->setCameraHeight(600);
	lastMinDistanceTime = 0;
	active = false;
	keyUpPressed = false;
	keyDownPressed = false;
	locked = false;
	lastCollisionDistance = 10000000;
}

void FollowCameraController::setDefaultSettings()
{
	_function_name("FollowCameraController::setDefaultSettings");
	this->noPitchCorrection = 1;
}

void FollowCameraController::setAlternateSettings()
{
	_function_name("FollowCameraController::setAlternateSettings");
	this->noPitchCorrection = 0;
}

void FollowCameraController::addModifier(ICameraStateModifier *modifier)
{
	_function_name("FollowCameraController::addModifier");

	(void)modifier;
	//if (((!(effectsEnabled)) || (this->modifiers.indexOf(modifier) > -1)))
		//return;

	//this->modifiers.push(modifier);
	//modifier.onAddedToController(this);
}

Tank *FollowCameraController::getTank()
{
	return this->_tank;
}

void FollowCameraController::setTank(Tank *value)
{
	this->_tank = value;
}

void FollowCameraController::initByTarget(Vector3 *targetPosition, Vector3 *targetDirection)
{
	_function_name("FollowCameraController::initByTarget");

	this->targetPosition.copy(targetPosition);
	this->targetDirection.copy(targetDirection);
	this->lastMinDistanceTime = 0;
	this->getCameraPositionData(targetPosition, targetDirection, false, 10000, &this->cameraPositionData);

	this->position.copy(&this->cameraPositionData.position);
	this->rotation.x = this->getPitchAngle(&this->cameraPositionData) - (0.5 * MyMath::PI);
	this->rotation.z = MyMath::atan2(-targetDirection->x, targetDirection->y);

	setPosition(&this->position);
	setOrientation(&this->rotation);
}

void FollowCameraController::initCameraComponents()
{
	_function_name("FollowCameraController::initCameraComponents");

	this->position.copy(&camera->pos);
	this->rotation.copy(&camera->rotation);
}

void FollowCameraController::activate()
{
	_function_name("FollowCameraController::activate");

	if (this->active)
		return;

	this->active = true;
}

void FollowCameraController::deactivate()
{
	_function_name("FollowCameraController::deactivate");

	if (!this->active)
		return;

	this->active = false;
	this->keyUpPressed = false;
	this->keyDownPressed = false;
}

void FollowCameraController::update(uint32_t time, uint32_t timeDelta)
{
	_function_name("FollowCameraController::update");

	(void)time;
	//ICameraStateModifier *modifier;
	if (this->_tank == nullptr)
		return;

	double dt = timeDelta * 0.001;
	if (dt > 0.1)
		dt = 0.1;

	this->updateHeight();

	if (!this->locked)
		this->updateTargetState();

	this->getCameraPositionData(&this->targetPosition, &this->targetDirection, true, dt, &this->cameraPositionData);

	Vector3 positionDelta;

	positionDelta.diff(&this->cameraPositionData.position, &this->position);
	double positionError = positionDelta.length();

	if (positionError > maxPositionError)
		this->linearSpeed = this->getLinearSpeed(positionError - maxPositionError);

	double distance = this->linearSpeed * dt;
	if (distance > positionError)
		distance = positionError;

	positionDelta.normalize();
	positionDelta.scale(distance);
	double targetPitchAngle = this->getPitchAngle(&this->cameraPositionData);
	double targetYawAngle = MyMath::atan2(-(this->targetDirection.x), this->targetDirection.y);
	double currentPitchAngle = MathUtils::clampAngle((this->rotation.x + (0.5 * MyMath::PI)));
	double currentYawAngle = MathUtils::clampAngle(this->rotation.z);
	double pitchError = MathUtils::clampAngleFast((targetPitchAngle - currentPitchAngle));
	this->pitchSpeed = this->getAngularSpeed(pitchError, this->pitchSpeed);
	double deltaPitch = (this->pitchSpeed * dt);
	if (((pitchError > 0) && (deltaPitch > pitchError)) || ((pitchError < 0) && (deltaPitch < pitchError)))
		deltaPitch = pitchError;

	double yawError = MathUtils::clampAngleFast((targetYawAngle - currentYawAngle));
	this->yawSpeed = this->getAngularSpeed(yawError, this->yawSpeed);
	double deltaYaw = this->yawSpeed * dt;

	if (((yawError > 0) && (deltaYaw > yawError)) || ((yawError < 0) && (deltaYaw < yawError)))
		deltaYaw = yawError;

	this->linearSpeed = this->snap(this->linearSpeed, 0, camSpeedThreshold);
	this->pitchSpeed = this->snap(this->pitchSpeed, 0, camSpeedThreshold);
	this->yawSpeed = this->snap(this->yawSpeed, 0, camSpeedThreshold);

	this->position.add(&positionDelta);
	this->rotation.x = (this->rotation.x + deltaPitch);
	this->rotation.z = (this->rotation.z + deltaYaw);
	Vector3 currentPosition, currentRotation;
	currentPosition.copy(&this->position);
	currentRotation.copy(&this->rotation);

	/*
	int i = 0;
	while (i < this->modifiers.length)
	{
		modifier = this->modifiers[i];
		if ((!(modifier.update(time, timeDelta, currentPosition, currentRotation))))
		{
			modifier.destroy();
			this->modifiers.splice(i, 1);
			i--;
		}
		i++;
	}
	*/


	setPosition(&currentPosition);
	setOrientation(&currentRotation);
}

void FollowCameraController::setLocked(bool locked)
{
	_function_name("FollowCameraController::setLocked");

	this->locked = locked;
}

double FollowCameraController::getCameraHeight()
{
	_function_name("FollowCameraController::getCameraHeight");

	return this->_cameraHeight;
}

void FollowCameraController::setCameraHeight(double value)
{
	_function_name("FollowCameraController::setCameraHeight");

	this->_cameraHeight = MathUtils::clamp(value, MIN_HEIGHT, MAX_HEIGHT);
	double d = this->getCamDistance(this->_cameraHeight);
	this->baseElevation = MyMath::atan2(this->_cameraHeight, d);
	this->cameraDistance = MyMath::sqrt(((this->_cameraHeight * this->_cameraHeight) + (d * d)));
	this->lastMinDistanceTime = 0;
}

void FollowCameraController::getCameraState(Vector3 *targetPosition, Vector3 *targetDirection, Vector3 *resultingPosition, Vector3 *resultingAngles)
{
	_function_name("FollowCameraController::getCameraState");

	this->getCameraPositionData(targetPosition, targetDirection, false, 10000, &this->cameraPositionData);
	resultingAngles->x = (this->getPitchAngle(&this->cameraPositionData) - (0.5 * MyMath::PI));
	resultingAngles->z = MyMath::atan2(-(targetDirection->x), targetDirection->y);
	resultingPosition->copy(&this->cameraPositionData.position);
}

void FollowCameraController::getCameraPositionData(Vector3 *targetPosition, Vector3 *targetDirection, bool useReboundDelay, double dt, CameraPositionData *result)
{
	_function_name("FollowCameraController::getCameraPositionData");

	(void)dt;
	//double angle;
	double actualElevation = this->baseElevation;
	double xyLength = MyMath::sqrt((targetDirection->x * targetDirection->x) + (targetDirection->y * targetDirection->y));

	Vector3 flatDirection;
	flatDirection.z = 0;

	if (xyLength < 1E-5)
	{
		flatDirection.x = 1;
		flatDirection.y = 0;
	}
	else
	{
		flatDirection.x = targetDirection->x / xyLength;
		flatDirection.y = targetDirection->y / xyLength;
	}

	result->extraPitch = 0;
	result->t = 1;
	double minCollisionDistance = this->cameraDistance;
	Vector3 rayOrigin;
	rayOrigin.copy(targetPosition);
	//elevationAngles[0] = actualElevation;
	Vector3 axis;
	axis.x = flatDirection.y;
	axis.y = -flatDirection.x;
	axis.z = 0;

	Vector3 rayDirection;
	flatDirection.reverse();

	//for each (angle in elevationAngles)
	{
		double angle = actualElevation;
		Matrix3 rotationMatrix;
		rotationMatrix.fromAxisAngle(&axis, -angle);
		rotationMatrix.transformVector(&flatDirection, &rayDirection);
		minCollisionDistance = this->getCollisionDistance(&rayOrigin, &rayDirection, this->cameraDistance, minCollisionDistance);
	}

	if (useReboundDelay)
	{
		int now = GetGameTime();
		if (minCollisionDistance <= (this->lastCollisionDistance + 0.001))
		{
			this->lastCollisionDistance = minCollisionDistance;
			this->lastMinDistanceTime = now;
		}
		else
		{
			if ((now - this->lastMinDistanceTime) < reboundDelay)
			{
				minCollisionDistance = this->lastCollisionDistance;
			}
			else
			{
				this->lastCollisionDistance = minCollisionDistance;
			}
		}
	}

	if (minCollisionDistance < this->cameraDistance)
	{
		result->t = minCollisionDistance / this->cameraDistance;
		if (minCollisionDistance < MIN_DISTANCE)
		{
			rayOrigin.addScaled(minCollisionDistance, &rayDirection);
			double delta = (MIN_DISTANCE - minCollisionDistance);
			RayIntersection rayIntersection;

			if (this->collisionDetector->intersectRay(&rayOrigin, &Vector3::Z_AXIS, this->cameraCollisionGroup, delta, nullptr, &rayIntersection))
			{
				delta = rayIntersection.t - 10;
				if (delta < 0)
				{
					delta = 0;
				}
			}

			result->position.copy(&rayOrigin);
			result->position.addScaled(delta, &Vector3::Z_AXIS);
		}
		else
		{
			result->position.copy(&rayOrigin);
			result->position.addScaled(minCollisionDistance, &rayDirection);
		}
	}
	else
	{
		result->position.copy(&rayOrigin);
		result->position.addScaled(this->cameraDistance, &rayDirection);
	}
}

double FollowCameraController::getCollisionDistance(const Vector3 *rayOrigin, const Vector3 *rayDirection, double rayLength, double minCollisionDistance)
{
	_function_name("FollowCameraController::getCollisionDistance");

	//return minCollisionDistance;

	RayIntersection rayIntersection;

	if (this->collisionDetector->intersectRay(rayOrigin, rayDirection, this->cameraCollisionGroup, rayLength, nullptr, &rayIntersection))
	{
		double distance = rayIntersection.t;

		if (distance < COLLISION_OFFSET)
			distance = 0.;
		else
			distance -= COLLISION_OFFSET;

		if (distance < minCollisionDistance)
			return distance;
	}

	return minCollisionDistance;
}

void FollowCameraController::updateTargetState()
{
	_function_name("FollowCameraController::updateTargetState");

	if (this->_tank == nullptr)
		return;

	TankSkin *skin = this->_tank->skin;
	if (skin == nullptr)
		return;

	Mesh *turretMesh = skin->turretMesh;
	if (turretMesh == nullptr)
		return;

	Matrix4 m;
	m.setMatrix(&turretMesh->position, &turretMesh->rotation);

#if 0
	m.transformVector(&Vector3::ZERO, &this->targetPosition);
	m.transformVector(&Vector3::Y_AXIS, &this->targetDirection);
	this->targetDirection.remove(&this->targetPosition);
#else

	Matrix3D m3;
	m3.set(m.ma, m.me, m.mi, 0, m.mb, m.mf, m.mj, 0, m.mc, m.mg, m.mk, 0, m.md, m.mh, m.ml, 1);

	const double vin[6] = {0, 0, 0, 0, 1, 0};
	double vout[6];
	m3.transformVectors_2(vin, vout);

	this->targetPosition.reset(vout[0], vout[1], vout[2]);
	this->targetDirection.reset(vout[3] - this->targetPosition.x, vout[4] - this->targetPosition.y, vout[5] - this->targetPosition.z);
#endif

}


void FollowCameraController::onKey(int key, bool down)
{
	switch (key)
	{
		case KEY_PRIOR:
		case KEY_Q:
			this->keyUpPressed = down;
			break;
		case KEY_NEXT:
		case KEY_E:
			this->keyDownPressed = down;
			break;
	}
}

void FollowCameraController::updateHeight()
{
	_function_name("FollowCameraController::updateHeight");

	if (this->keyUpPressed == this->keyDownPressed)
		return;

	int heightChangeDir = this->keyUpPressed ? 1 : -1;
	this->setCameraHeight(this->_cameraHeight + (heightChangeDir * HEIGHT_CHANGE_STEP));
}

double FollowCameraController::getCamDistance(double h)
{
	_function_name("FollowCameraController::getCamDistance");

	double d1 = 600.;
	double d2 = 1300.;
	double hMin = 200.;
	double hMax = 3300.;
	double hMid = 0.5 * (hMax + hMin);
	double a = hMid;
	double b = d2;
	double v = hMin - hMid;
	double k = (d2 - d1) / (v * v);
	return ((-k * (h - a)) * (h - a)) + b;
}

double FollowCameraController::getLinearSpeed(double positionError)
{
	return 3 * positionError;
}

double FollowCameraController::getAngularSpeed(double angleError, double currentSpeed)
{
	_function_name("FollowCameraController::getAngularSpeed");

	double k = 3.;
	if (angleError < -maxAngleError)
		return k * (angleError + maxAngleError);

	if (angleError > maxAngleError)
		return k * (angleError - maxAngleError);

	return currentSpeed;
}

double FollowCameraController::snap(double value, double snapValue, double epsilon)
{
	_function_name("FollowCameraController::snap");

	if ((value > (snapValue - epsilon)) && (value < (snapValue + epsilon)))
		return snapValue;

	return value;
}

double FollowCameraController::getPitchAngle(CameraPositionData *cameraPositionData)
{
	_function_name("FollowCameraController::getPitchAngle");

	double angle = (this->baseElevation - fixedPitch);
	if (angle < 0.)
		angle = 0.;

	double t = cameraPositionData->t;
	if ((t >= 1.) || (angle < MIN_CAMERA_ANGLE) || (this->noPitchCorrection == 1.))
		return cameraPositionData->extraPitch - angle;

	//double k = 1.5;
	return cameraPositionData->extraPitch - MyMath::atan2((t * this->_cameraHeight), ((pitchCorrectionCoeff * this->_cameraHeight) * ((1. / MyMath::tan(angle)) - ((1 - t) / MyMath::tan(this->baseElevation)))));
}

double FollowCameraController::moveValueTowards(double value, double targetValue, double delta)
{
	_function_name("FollowCameraController::moveValueTowards");

	if (value < targetValue) {
		value = value + delta;
		return (value > targetValue) ? targetValue : value;
	}

	if (value > targetValue) {
		value = value - delta;
		return (value < targetValue) ? targetValue : value;
	}

	return value;
}
