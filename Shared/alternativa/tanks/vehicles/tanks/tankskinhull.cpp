#include "tankskinhull.h"


TankSkinHull::TankSkinHull(std::shared_ptr<Mesh> &mesh2, Vector3 *turretMountPoint) :
	TankSkinPart(mesh2, true)
{
	this->mesh1 = mesh2;
	this->turretMountPoint.copy(turretMountPoint);
	//mesh2->name = Object3DNames.TANK_PART;
}

Mesh *TankSkinHull::getMesh()
{
	return this->mesh1.get();
}
