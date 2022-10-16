#ifndef ICOMMONSHOOTSFX_H
#define ICOMMONSHOOTSFX_H

class ClientObjectWeapon;
class Vector3;
class Object3D;
class Camera3D;
class EffectsPair;

class ICommonShootSFX
{
public:
	virtual void createShotEffects(ClientObjectWeapon*, const Vector3*, Object3D*, Camera3D*, EffectsPair*) = 0;
	virtual void createExplosionEffects(ClientObjectWeapon*, const Vector3*, Camera3D*, double, EffectsPair*) = 0;
};

#endif // ICOMMONSHOOTSFX_H
