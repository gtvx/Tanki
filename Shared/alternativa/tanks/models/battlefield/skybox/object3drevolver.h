#ifndef OBJECT3DREVOLVER_H
#define OBJECT3DREVOLVER_H

class Object3D;
class Vector3;
class Quaternion;

#include "alternativa/math/vector3.h"
#include "alternativa/math/quaternion.h"

class Object3DRevolver
{
	Object3D *object;
	Vector3 axis;
	double angularSpeed;
	Quaternion orientation;

public:

	Object3DRevolver(Object3D*, Vector3*, double);
	void setAxis(double x, double y, double z);
	void setAngularSpeed(double v);
	void render(int, int);
};

#endif // OBJECT3DREVOLVER_H
