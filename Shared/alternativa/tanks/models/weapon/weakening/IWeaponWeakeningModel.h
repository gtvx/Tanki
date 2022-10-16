#ifndef IWEAPONWEAKENINGMODEL_H
#define IWEAPONWEAKENINGMODEL_H

class ClientObjectWeapon;

class IWeaponWeakeningModel
{
public:
	virtual double getImpactCoeff(ClientObjectWeapon*, double) = 0;
	virtual double getFullDamageRadius(ClientObjectWeapon*) = 0;
};

#endif // IWEAPONWEAKENINGMODEL_H
