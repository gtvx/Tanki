#include "fallcontroller.h"
#include "bonusconst.h"
#include "mymath.h"
#include "parabonus2.h"
#include "alternativa/math/matrix4.h"
#include "bonustrigger.h"
#include "alternativa/engine3d/core/object3d.h"
#include "bonusmesh.h"
#include "parachute2.h"
#include "cords2.h"


static const double MAX_ANGLE_X = 0.3;
static const double ANGLE_X_FREQ = 3.4;
static const double MAX_ANGLE_X_PARACHUTE = 0.15;
static const double ANGLE_X_FREQ_PARACHUTE = 1.3;


FallController::FallController(ParaBonus2 *battleBonus)
{
	x = 0.;
	y = 0.;
	t0 = 0.;
	fallSpeed = 0;
	time = 0;
	minPivotZ = 0;
	this->battleBonus = battleBonus;

	interpolatedVector.reset();
	interpolatedParachuteVector.reset();
	interpolatedMatrix.copy(&Matrix3::IDENTITY);
	interpolatedParachuteMatrix.copy(&Matrix3::IDENTITY);
}

void FallController::init(const Vector3 *spawnPosition, double fallSpeed, double minPivotZ, double t0, double startTime, double startingAngleZ)
{
	this->x = spawnPosition->x;
	this->y = spawnPosition->y;
	this->newState.pivotZ = (spawnPosition->z + BonusConst::BONUS_OFFSET_Z) - (fallSpeed * startTime);
	this->newState.angleZ = startingAngleZ + (BonusConst::ANGULAR_SPEED_Z * startTime);
	this->newStateParachute.pivotZ = ((spawnPosition->z + BonusConst::BONUS_OFFSET_Z) - (fallSpeed * startTime));
	this->newStateParachute.angleZ = (startingAngleZ + (BonusConst::ANGULAR_SPEED_Z * startTime));
	this->fallSpeed = fallSpeed;
	this->minPivotZ = minPivotZ;
	this->t0 = t0;
	this->time = startTime;
}

void FallController::start()
{
}

void FallController::runBeforePhysicsUpdate(double dt)
{
	dt = (dt * 0.001);
	this->oldState.copy(&this->newState);
	this->oldStateParachute.copy(&this->newStateParachute);
	this->time += dt;
	this->newState.pivotZ = this->newState.pivotZ - (this->fallSpeed * dt);

	this->newState.angleX = MAX_ANGLE_X * MyMath::sin(ANGLE_X_FREQ * (this->t0 + this->time));

	this->newState.angleZ = this->newState.angleZ + ((BonusConst::ANGULAR_SPEED_Z + 0.2) * dt);
	this->newStateParachute.pivotZ = this->newStateParachute.pivotZ - (this->fallSpeed * dt);
	this->newStateParachute.angleX = MAX_ANGLE_X_PARACHUTE * MyMath::sin(ANGLE_X_FREQ_PARACHUTE * (this->t0 + this->time));
	this->newStateParachute.angleZ = this->newStateParachute.angleZ + (BonusConst::ANGULAR_SPEED_Z * dt);

	if (this->newState.pivotZ <= this->minPivotZ)
	{
		this->newState.pivotZ = this->minPivotZ;
		this->newState.angleX = 0;
		this->newStateParachute.pivotZ = this->minPivotZ;
		this->newStateParachute.angleX = 0;
		this->interpolatePhysicsState(1);
		this->render();
		this->battleBonus->onStaticCollision();

	}

	this->updateTrigger();

}

void FallController::updateTrigger()
{
	Matrix4 m;
	m.m03 = m.m13 = m.m23 = 0.;
	m.setRotationMatrix(this->newState.angleX, 0, this->newState.angleZ);
	Vector3 v;
	m.transformVector(&Vector3::DOWN, &v);
	v.scale(BonusConst::BONUS_OFFSET_Z);
	this->battleBonus->trigger->update((this->x + v.x), (this->y + v.y), (this->newState.pivotZ + v.z), this->newState.angleX, 0, this->newState.angleZ);
}

void FallController::interpolatePhysicsState(double interpolationCoeff)
{
	this->interpolatedParachuteState.interpolate(&this->oldStateParachute, &this->newStateParachute, interpolationCoeff);
	this->interpolatedParachuteMatrix.setRotationMatrix(this->interpolatedParachuteState.angleX, 0, this->interpolatedParachuteState.angleZ);
	this->interpolatedParachuteMatrix.transformVector(&Vector3::DOWN, &this->interpolatedParachuteVector);
	this->interpolatedState.interpolate(&this->oldState, &this->newState, interpolationCoeff);
	this->interpolatedMatrix.setRotationMatrix(this->interpolatedState.angleX, 0, this->interpolatedState.angleZ);
	this->interpolatedMatrix.transformVector(&Vector3::DOWN, &this->interpolatedVector);
}

void FallController::render()
{
	this->setObjectTransform(this->battleBonus->parachute.get(), BonusConst::PARACHUTE_OFFSET_Z, &this->interpolatedParachuteVector);
	this->setObjectTransform(this->battleBonus->skin.get(), BonusConst::BONUS_OFFSET_Z, &this->interpolatedVector);
	this->battleBonus->cordsMesh->updateVertices();
}

void FallController::setObjectTransform(Object3D *object, double objectOffset, const Vector3 *offsetVector)
{
	if (object != this->battleBonus->parachute.get())
	{
		object->rotation.x = this->interpolatedState.angleX;
		object->rotation.z = this->interpolatedState.angleZ;
		object->position.x = this->x + (50. * offsetVector->x);
		object->position.y = this->y + (50. * offsetVector->y);
		object->position.z = this->interpolatedState.pivotZ + (objectOffset * offsetVector->z);
	}
	else
	{
		object->rotation.x = this->interpolatedParachuteState.angleX;
		object->rotation.z = this->interpolatedParachuteState.angleZ;
		object->position.x = this->x + (objectOffset * offsetVector->x);
		object->position.y = this->y + (objectOffset * offsetVector->y);
		object->position.z = this->interpolatedParachuteState.pivotZ + (objectOffset * offsetVector->z);
	}
}
