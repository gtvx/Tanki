#ifndef ITHUNDERSFXMODEL_H
#define ITHUNDERSFXMODEL_H

class EffectsPair;
class Object3D;
class Vector3;
class ClientObjectWeapon;

class IThunderSFXModel
{
public:
	virtual void createShotEffects(ClientObjectWeapon*, const Vector3*, Object3D*, EffectsPair*) = 0;
	virtual void createExplosionEffects(ClientObjectWeapon*, const Vector3*, EffectsPair*) = 0;
};

#endif // ITHUNDERSFXMODEL_H
