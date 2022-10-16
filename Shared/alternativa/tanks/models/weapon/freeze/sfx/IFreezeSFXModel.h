#ifndef IFREEZESFXMODEL_H
#define IFREEZESFXMODEL_H

class TankData;
class WeaponCommonData;

class IFreezeSFXModel
{
public:
	virtual void createEffects(TankData*, WeaponCommonData*) = 0;
	virtual void destroyEffects(TankData*) = 0;
};

#endif // IFREEZESFXMODEL_H
