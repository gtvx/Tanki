#ifndef WEAPONUTILS_H
#define WEAPONUTILS_H

class Object3D;
class Vector3;
class AllGlobalGunParams;

class WeaponUtils
{
public:
	static void calculateMainGunParams(const Object3D *turret, const Vector3 *localMuzzlePosition, AllGlobalGunParams*);
	static void calculateGunParams(const Object3D *turret, const Vector3 *localMuzzlePosition, Vector3 *globalMuzzlePosition, Vector3 *barrelOrigin, Vector3 *turretAxisX, Vector3 *turretAxisY);
	static void calculateGunParamsAux(const Object3D *turret, const Vector3 *localMuzzlePosition, Vector3 *globalMuzzlePosition, Vector3 *turretAxisY);
};

#endif // WEAPONUTILS_H
