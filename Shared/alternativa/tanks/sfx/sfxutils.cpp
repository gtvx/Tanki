#include "sfxutils.h"
#include "alternativa/engine3d/core/object3d.h"
#include "alternativa/math/matrix3.h"
#include "mymath.h"


void SFXUtils::alignObjectPlaneToView(Object3D *object, const Vector3 *objectPosition, const Vector3 *objectDirection, const Vector3 *cameraPosition)
{
	double angle, dot;

	Vector3 axis1, axis2, objectAxis, targetAxisZ, eulerAngles;
	Matrix3 matrix1, matrix2;

	if (((objectDirection->y < -0.99999) || (objectDirection->y > 0.99999)))
	{
		axis1.x = 0;
		axis1.y = 0;
		axis1.z = 1;
		angle = ((objectDirection->y < 0) ? MyMath::PI : 0);
	}
	else
	{
		axis1.x = objectDirection->z;
		axis1.y = 0;
		axis1.z = -objectDirection->x;
		axis1.normalize();
		angle = MyMath::acos(objectDirection->y);
	}

	matrix1.fromAxisAngle(&axis1, angle);
	targetAxisZ.x = cameraPosition->x - objectPosition->x;
	targetAxisZ.y = cameraPosition->y - objectPosition->y;
	targetAxisZ.z = cameraPosition->z - objectPosition->z;
	dot = (((targetAxisZ.x * objectDirection->x) + (targetAxisZ.y * objectDirection->y)) + (targetAxisZ.z * objectDirection->z));
	targetAxisZ.x = (targetAxisZ.x - (dot * objectDirection->x));
	targetAxisZ.y = (targetAxisZ.y - (dot * objectDirection->y));
	targetAxisZ.z = (targetAxisZ.z - (dot * objectDirection->z));
	targetAxisZ.normalize();
	matrix1.transformVector(&Vector3::Z_AXIS, &objectAxis);
	dot = ((objectAxis.x * targetAxisZ.x) + (objectAxis.y * targetAxisZ.y)) + (objectAxis.z * targetAxisZ.z);
	axis2.x = ((objectAxis.y * targetAxisZ.z) - (objectAxis.z * targetAxisZ.y));
	axis2.y = ((objectAxis.z * targetAxisZ.x) - (objectAxis.x * targetAxisZ.z));
	axis2.z = ((objectAxis.x * targetAxisZ.y) - (objectAxis.y * targetAxisZ.x));
	axis2.normalize();
	angle = MyMath::acos(dot);
	matrix2.fromAxisAngle(&axis2, angle);
	matrix1.append(&matrix2);
	matrix1.getEulerAngles(&eulerAngles);
	object->rotation.x = eulerAngles.x;
	object->rotation.y = eulerAngles.y;
	object->rotation.z = eulerAngles.z;
	object->position.x = objectPosition->x;
	object->position.y = objectPosition->y;
	object->position.z = objectPosition->z;
}
