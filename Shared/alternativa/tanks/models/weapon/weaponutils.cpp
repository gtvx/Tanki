#include "weaponutils.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/engine3d/core/object3d.h"
#include "AllGlobalGunParams.h"
#include "_global.h"


//private static var turretMatrix:Matrix4 = new Matrix4();
//private static var localBarrelOrigin:Vector3 = new Vector3();


void WeaponUtils::calculateMainGunParams(const Object3D *turret, const Vector3 *localMuzzlePosition, AllGlobalGunParams *gun)
{
	_function_name("WeaponUtils::calculateMainGunParams");
	turret->matrix.transformVector(localMuzzlePosition, &gun->muzzlePosition);
	Vector3 localBarrelOrigin;
	localBarrelOrigin.x = localMuzzlePosition->x;
	localBarrelOrigin.y = 0.;
	localBarrelOrigin.z = localMuzzlePosition->z;
	turret->matrix.transformVector(&localBarrelOrigin, &gun->barrelOrigin);
	gun->elevationAxis.x = turret->matrix.a;
	gun->elevationAxis.y = turret->matrix.e;
	gun->elevationAxis.z = turret->matrix.i;
	gun->direction.x = turret->matrix.b;
	gun->direction.y = turret->matrix.f;
	gun->direction.z = turret->matrix.j;
}

void WeaponUtils::calculateGunParams(const Object3D *turret, const Vector3 *localMuzzlePosition, Vector3 *globalMuzzlePosition, Vector3 *barrelOrigin, Vector3 *turretAxisX, Vector3 *turretAxisY)
{
	Matrix4 turretMatrix;
	turretMatrix.setMatrix(&turret->position, &turret->rotation);
	turretMatrix.transformVector(localMuzzlePosition, globalMuzzlePosition);
	Vector3 localBarrelOrigin;
	localBarrelOrigin.x = localMuzzlePosition->x;
	localBarrelOrigin.y = 0.;
	localBarrelOrigin.z = localMuzzlePosition->z;
	turretMatrix.transformVector(&localBarrelOrigin, barrelOrigin);
	turretAxisX->x = turretMatrix.a;
	turretAxisX->y = turretMatrix.e;
	turretAxisX->z = turretMatrix.i;
	turretAxisY->x = turretMatrix.b;
	turretAxisY->y = turretMatrix.f;
	turretAxisY->z = turretMatrix.j;
}

void WeaponUtils::calculateGunParamsAux(const Object3D *turret, const Vector3 *localMuzzlePosition, Vector3 *globalMuzzlePosition, Vector3 *turretAxisY)
{
	_function_name("WeaponUtils::calculateGunParamsAux");
	Matrix4 turretMatrix;
	turretMatrix.setMatrix(&turret->position, &turret->rotation);
	turretMatrix.transformVector(localMuzzlePosition, globalMuzzlePosition);
	turretAxisY->x = turretMatrix.b;
	turretAxisY->y = turretMatrix.f;
	turretAxisY->z = turretMatrix.j;
}
