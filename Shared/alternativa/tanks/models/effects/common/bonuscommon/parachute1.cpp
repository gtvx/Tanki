#include "parachute1.h"
#include "alternativa/physics/physicsutils.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "mymath.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"


const Body::T *Parachute1::TYPE = new Body::T("Parachute");


Parachute1::Parachute1(const T *type, double mass, double radius, double height, double k1, double k2,
					 std::shared_ptr<Mesh> &outerMeshRef, std::shared_ptr<Mesh> &innerMeshRef) :
	Body3D(type, 1. / mass, &Matrix3::IDENTITY)
{
	(void)innerMeshRef;
	PhysicsUtils::getCylinderInvInertia(mass, radius, height, &invInertia);
	this->k1 = k1;
	this->k2 = k2;
	this->outerMesh = Mesh_clone(outerMeshRef);
}

void Parachute1::beforePhysicsStep(double dt)
{
	(void)dt;

	double dx = state.velocity.x;
	double dy = state.velocity.y;
	double dz = state.velocity.z;
	double speed = MyMath::sqrt((dx * dx) + (dy * dy) + (dz * dz));
	if (speed < 0.001)
		return;

	dx = (dx / speed);
	dy = (dy / speed);
	dz = (dz / speed);
	double mag = (((this->k1 * speed) * speed) + (((this->k2 * speed) * speed) * speed));
	Vector3 force;
	force.x = -dx * mag;
	force.y = -dy * mag;
	force.z = -dz * mag;
	addForce(&force);
}

void Parachute1::addToContainer(Scene3DContainer *container)
{
	if (container != nullptr)
		container->addChild(this->outerMesh.get());
}

void Parachute1::removeFromContainer()
{
	this->outerMesh->removeFromParent();
}

void Parachute1::updateSkin(double t)
{
	if (this->outerMesh == nullptr)
		return;

	Vector3 v;
	Quaternion o;

	interpolate(t, &v, &o);
	this->outerMesh->position.copy(&v);
	o.normalize();
	o.getEulerAngles(&v);
	this->outerMesh->rotation.copy(&v);
}
