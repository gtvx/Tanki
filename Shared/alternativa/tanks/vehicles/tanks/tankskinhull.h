#ifndef TANKSKINHULL_H
#define TANKSKINHULL_H

#include "tankskinpart.h"
#include "alternativa/math/vector3.h"

class Vector3;

class TankSkinHull : public TankSkinPart
{
public:

	Vector3 turretMountPoint;
	std::shared_ptr<Mesh> mesh1;

	TankSkinHull(std::shared_ptr<Mesh> &mesh2, Vector3 *turretMountPoint);


protected:
	Mesh* getMesh() override;

};

#endif // TANKSKINHULL_H
