#include "weaponweakeningmodel.h"
#include "WeaponWeakeningData.h"
#include "_global.h"
#include "mymath.h"
#include "alternativa/object/clientobjectweapon.h"



void WeaponWeakeningModel::initObject(ClientObjectWeapon *clientObject, double maximumDamageRadius, double minimumDamagePercent, double minimumDamageRadius)
{
	_function_name("WeaponWeakeningModel::initObject");
	std::shared_ptr<WeaponWeakeningData> data = std::make_shared<WeaponWeakeningData>();
	data->maximumDamageRadius = MyMath::isNaN(maximumDamageRadius) ? 0 : maximumDamageRadius;
	data->minimumDamageRadius = MyMath::isNaN(minimumDamageRadius) ? 1 : minimumDamageRadius;
	data->minimumDamagePercent = MyMath::isNaN(minimumDamagePercent) ? 0 : minimumDamagePercent;
	data->falloffInterval = data->minimumDamageRadius - data->maximumDamageRadius;
	if (data->minimumDamagePercent > 100)
		data->minimumDamagePercent = 100;
	clientObject->weaponWeakeningData = data;
}

double WeaponWeakeningModel::getImpactCoeff(ClientObjectWeapon *clientObject, double distance)
{
	_function_name("WeaponWeakeningModel::getImpactCoeff");
	WeaponWeakeningData *data = clientObject->weaponWeakeningData.get();
	if (data->falloffInterval <= 0)
		return 1.;

	if (distance <= data->maximumDamageRadius)
		return 1.;

	if (distance >= data->minimumDamageRadius)
		return 0.01 * data->minimumDamagePercent;

	return 0.01 * data->minimumDamagePercent + (((data->minimumDamageRadius - distance) * (100 - data->minimumDamagePercent)) / data->falloffInterval);
}

double WeaponWeakeningModel::getFullDamageRadius(ClientObjectWeapon *clientObject)
{
	_function_name("WeaponWeakeningModel::getFullDamageRadius");
	WeaponWeakeningData *weaponWeakeningData = clientObject->weaponWeakeningData.get();
	if (weaponWeakeningData == nullptr)
		throw 25433;
	return weaponWeakeningData->maximumDamageRadius * 100.;
}

