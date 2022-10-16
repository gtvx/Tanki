#ifndef IWEAPONCOMMONMODEL_H
#define IWEAPONCOMMONMODEL_H

class WeaponCommonData;
class Vector3;
class Tank;
class Camera3D;
class ClientObjectWeapon;
class TankData;


class IWeaponCommonModel
{
public:
	virtual WeaponCommonData* getCommonData(ClientObjectWeapon*) = 0;
	virtual void createShotEffects(ClientObjectWeapon*, Tank*, int, const Vector3*, const Vector3*) = 0;
	virtual void createExplosionEffects(ClientObjectWeapon*, Camera3D*, bool, const Vector3*, const Vector3*, TankData*, double) = 0;
};


#endif // IWEAPONCOMMONMODEL_H
