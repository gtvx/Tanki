#include "tankskinturret.h"


TankSkinTurret::TankSkinTurret(std::shared_ptr<Mesh> &mesh,
							   const Vector3 *flagMountPoint,
							   const Vector3 *muzzle) :
	TankSkinPart(mesh, false)
{
	this->flagMountPoint.copy(flagMountPoint);
	this->mesh1 = mesh;
	this->shaftMuzzle.copy(muzzle);
	//mesh->name = Object3DNames::TANK_PART;
}

TankSkinTurret::~TankSkinTurret()
{

}

Mesh* TankSkinTurret::getMesh()
{
	return this->mesh1.get();
}
