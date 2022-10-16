#include "suspensionray.h"
#include "alternativa/physics/body.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/physics/physicsscene.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "raypredicate.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "mymath.h"
#include "SuspensionData.h"
#include "_global.h"



SuspensionRay::SuspensionRay(Body *body, Vector3 *relPos, Vector3 *relDir)
{
	this->lastCollided = false;
	this->body = body;
	this->relPos.copy(relPos);
	this->relDir.copy(relDir);
	this->predicate = new RayPredicate(body);
	this->collisionGroup = 0;
}

SuspensionRay::~SuspensionRay()
{
	delete this->predicate;
}

bool SuspensionRay::calculateIntersection(double maxLength)
{
	this->body->baseMatrix.transformVector(&this->relDir, &this->worldDir);
	this->body->baseMatrix.transformVector(&this->relPos, &this->worldPos);
	Vector3 &p = this->body->state.pos;
	this->worldPos.x = this->worldPos.x + p.x;
	this->worldPos.y = this->worldPos.y + p.y;
	this->worldPos.z = this->worldPos.z + p.z;
	if (this->lastCollided == true)
	{
		this->prevDisplacement = maxLength - this->lastIntersection.t;
	}

	return this->lastCollided = this->body->world->collisionDetector->intersectRay(&this->worldPos, &this->worldDir, this->collisionGroup, maxLength, this->predicate, &this->lastIntersection);
}



void SuspensionRay::addForce(double dt, double throttle, double maxSpeed, int slipTerm, double springCoeff, SuspensionData *data, bool fwdBrake)
{
	if (!this->lastCollided)
		return;

	const Vector3 &_groundUp = this->lastIntersection.normal;

	Vector3 _v;
	_v.x = this->body->baseMatrix.b;
	_v.y = this->body->baseMatrix.f;
	_v.z = this->body->baseMatrix.j;

	Vector3 _groundRight;
	_groundRight.x = (_v.y * _groundUp.z) - (_v.z * _groundUp.y);
	_groundRight.y = (_v.z * _groundUp.x) - (_v.x * _groundUp.z);
	_groundRight.z = (_v.x * _groundUp.y) - (_v.y * _groundUp.x);

	double len = (_groundRight.x * _groundRight.x) + (_groundRight.y * _groundRight.y) + (_groundRight.z * _groundRight.z);

	if (len == 0)
	{
		_groundRight.x = 1;
	}
	else
	{
		len = MyMath::sqrt(len);
		_groundRight.x = _groundRight.x / len;
		_groundRight.y = _groundRight.y / len;
		_groundRight.z = _groundRight.z / len;
	}

	Vector3 _groundForward;
	_groundForward.x = (_groundUp.y * _groundRight.z) - (_groundUp.z * _groundRight.y);
	_groundForward.y = (_groundUp.z * _groundRight.x) - (_groundUp.x * _groundRight.z);
	_groundForward.z = (_groundUp.x * _groundRight.y) - (_groundUp.y * _groundRight.x);

	const BodyState &state = this->body->state;
	_v.x = this->lastIntersection.pos.x - state.pos.x;
	_v.y = this->lastIntersection.pos.y - state.pos.y;
	_v.z = this->lastIntersection.pos.z - state.pos.z;

	const Vector3 &rot = state.rotation;
	Vector3 _relVel;
	_relVel.x = ((rot.y * _v.z) - (rot.z * _v.y)) + state.velocity.x;
	_relVel.y = ((rot.z * _v.x) - (rot.x * _v.z)) + state.velocity.y;
	_relVel.z = ((rot.x * _v.y) - (rot.y * _v.x)) + state.velocity.z;

	if (this->lastIntersection.primitive->body != nullptr)
	{
		const BodyState &bState = this->lastIntersection.primitive->body->state;
		Vector3 _v;
		_v.x = this->lastIntersection.pos.x - bState.pos.x;
		_v.y = this->lastIntersection.pos.y - bState.pos.y;
		_v.z = this->lastIntersection.pos.z - bState.pos.z;
		const Vector3 &rot = bState.rotation;
		_relVel.x -= ((rot.y * _v.z) - (rot.z * _v.y)) + bState.velocity.x;
		_relVel.y -= ((rot.z * _v.x) - (rot.x * _v.z)) + bState.velocity.y;
		_relVel.z -= ((rot.x * _v.y) - (rot.y * _v.x)) + bState.velocity.z;
	}

	double relSpeed = MyMath::sqrt((_relVel.x * _relVel.x) + (_relVel.y * _relVel.y) + (_relVel.z * _relVel.z));
	double fwdSpeed = (_relVel.x * _groundForward.x) + (_relVel.y * _groundForward.y) + (_relVel.z * _groundForward.z);
	if (((throttle > 0.) && (fwdSpeed < maxSpeed)) || ((throttle < 0.) && (-(fwdSpeed) < maxSpeed)))
	{
		Vector3 _v;
		_v.x = this->worldPos.x + (data->forceOffset * this->worldDir.x);
		_v.y = this->worldPos.y + (data->forceOffset * this->worldDir.y);
		_v.z = this->worldPos.z + (data->forceOffset * this->worldDir.z);
		Vector3 _force;
		_force.x = throttle * _groundForward.x;
		_force.y = throttle * _groundForward.y;
		_force.z = throttle * _groundForward.z;
		this->body->addWorldForce(&_v, &_force);
	}

	Vector3 _worldUp;
	_worldUp.x = this->body->baseMatrix.c;
	_worldUp.y = this->body->baseMatrix.g;
	_worldUp.z = this->body->baseMatrix.k;
	double t = this->lastIntersection.t;
	double currDisplacement = data->rayLength - t;
	double springForce = (springCoeff * currDisplacement) * (((_worldUp.x * this->lastIntersection.normal.x) + (_worldUp.y * this->lastIntersection.normal.y)) + (_worldUp.z * this->lastIntersection.normal.z));
	double upSpeed = (currDisplacement - this->prevDisplacement) / dt;

	springForce = springForce + (upSpeed * data->dampingCoeff);

	if (springForce < 0.)
	{
		springForce = 0;
	}

	Vector3 _force;
	_force.copy(&this->worldDir);
	_force.scale(-springForce);

	if (relSpeed > 0.001)
	{
		double slipSpeed = _relVel.dot(&_groundRight);
		double sideFriction = (((slipTerm == 0) || ((slipSpeed >= 0) && (slipTerm > 0))) || ((slipSpeed <= 0) && (slipTerm < 0))) ? data->staticFriction : (2. * data->dynamicFriction);

		double frictionForce = ((sideFriction * springForce) * slipSpeed) / relSpeed;

		if (slipSpeed > -data->smallVel && slipSpeed < data->smallVel)
		{
			frictionForce *= slipSpeed / data->smallVel;
			if (slipSpeed < 0)
			{
				frictionForce = -(frictionForce);
			}
		}
		_force.x -= frictionForce * _groundRight.x;
		_force.y -= frictionForce * _groundRight.y;
		_force.z -= frictionForce * _groundRight.z;

		double fwdFriction;
		if (fwdBrake)
		{
			fwdFriction = 0.3 * data->staticFriction;
		}
		else
		{
			if ((fwdSpeed * throttle) <= 0)
			{
				fwdFriction = 0.5 * data->staticFriction;
			}
			else
			{
				fwdFriction = data->dynamicFriction;
			}
		}
		frictionForce = ((fwdFriction * springForce) * fwdSpeed) / relSpeed;
		if (fwdSpeed > -data->smallVel && fwdSpeed < data->smallVel)
		{
			frictionForce = frictionForce * (fwdSpeed / data->smallVel);
			if (fwdSpeed < 0)
			{
				frictionForce = -frictionForce;
			}
		}
		_force.x = _force.x - (frictionForce * _groundForward.x);
		_force.y = _force.y - (frictionForce * _groundForward.y);
		_force.z = _force.z - (frictionForce * _groundForward.z);
	}
	_v.x = this->worldPos.x + (data->forceOffset * this->worldDir.x);
	_v.y = this->worldPos.y + (data->forceOffset * this->worldDir.y);
	_v.z = this->worldPos.z + (data->forceOffset * this->worldDir.z);

	this->body->addWorldForce(&_v, &_force);
}
