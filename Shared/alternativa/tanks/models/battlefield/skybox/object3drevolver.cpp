#include "object3drevolver.h"
#include "alternativa/engine3d/core/object3d.h"
#include "mymath.h"




Object3DRevolver::Object3DRevolver(Object3D *object, Vector3 *param2, double v)
{
	this->object = object;
	this->axis.copy(param2);
	this->axis.normalize();
	this->angularSpeed = v / 1000.;
	this->orientation.setFromEulerAnglesXYZ(object->rotation.x, object->rotation.y, object->rotation.z);
}

void Object3DRevolver::setAxis(double x, double y, double z)
{
	this->axis.reset(x, y, z);
	this->axis.normalize();
}

void Object3DRevolver::setAngularSpeed(double v)
{
	this->angularSpeed = v / 1000.;
}

void Object3DRevolver::render(int, int v)
{
	if (this->angularSpeed != 0)
	{
		this->orientation.addScaledVector(&this->axis, this->angularSpeed * v);
		this->orientation.getEulerAngles(&this->object->rotation);
	}
}
