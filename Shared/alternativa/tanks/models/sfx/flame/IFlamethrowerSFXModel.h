#ifndef IFLAMETHROWERSFXMODEL_H
#define IFLAMETHROWERSFXMODEL_H

class EffectsPair;
class TankData;
class Vector3;
class Object3D;
class IWeaponWeakeningModel;

class IFlamethrowerSFXModel
{
public:
	virtual void getSpecialEffects(TankData*, const Vector3*, Object3D*, IWeaponWeakeningModel*, EffectsPair*) = 0;
};

#endif // IFLAMETHROWERSFXMODEL_H
