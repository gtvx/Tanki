#ifndef WEAPONWEAKENINGMODEL_H
#define WEAPONWEAKENINGMODEL_H

#include "IWeaponWeakeningModel.h"

//extends WeaponWeakeningModelBase implements IWeaponWeakeningModelBase, IWeaponWeakeningModel
class WeaponWeakeningModel
{
public:
	static void initObject(ClientObjectWeapon *clientObject, double maximumDamageRadius, double minimumDamagePercent, double minimumDamageRadius);
	static double getImpactCoeff(ClientObjectWeapon *clientObject, double distance);
	static double getFullDamageRadius(ClientObjectWeapon *clientObject);
};

#endif // WEAPONWEAKENINGMODEL_H
