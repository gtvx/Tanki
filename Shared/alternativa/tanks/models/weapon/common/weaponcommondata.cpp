#include "weaponcommondata.h"
#include <QVector>

WeaponCommonData::WeaponCommonData()
{
	kickback = 0;
	impactCoeff = 0;
	impactForce = 0;
	turretRotationAccel = 0;
	turretRotationSpeed = 0;
	_muzzles = nullptr;
	currBarrel = 0;
	muzzle_length = 0;
	weaponController = nullptr;
}

void WeaponCommonData::setMuzzles(std::shared_ptr<QVector<std::shared_ptr<Vector3> > > &muzzles)
{
	this->_muzzles = muzzles;
	this->muzzle_length = muzzles->length();
}

const Vector3* WeaponCommonData::getMuzzle(int index)
{
	return _muzzles->at(index).get();
}

