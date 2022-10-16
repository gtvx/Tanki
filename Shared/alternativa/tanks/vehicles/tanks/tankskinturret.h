#ifndef TANKSKINTURRET_H
#define TANKSKINTURRET_H

#include "tankskinpart.h"
#include "alternativa/math/vector3.h"

class Vector3;

class TankSkinTurret : public TankSkinPart
{
public:
	TankSkinTurret();
	Vector3 flagMountPoint;
	Vector3 shaftMuzzle;
	std::shared_ptr<Mesh> mesh1;

	TankSkinTurret(std::shared_ptr<Mesh> &mesh,
				   const Vector3 *flagMountPoint,
				   const Vector3 *muzzle);
	virtual ~TankSkinTurret();

protected:
	Mesh* getMesh() override;
};

#endif // TANKSKINTURRET_H
