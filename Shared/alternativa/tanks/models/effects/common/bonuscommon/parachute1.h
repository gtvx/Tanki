#ifndef PARACHUTE1_H
#define PARACHUTE1_H

#include "alternativa/physics/rigid/body3d.h"
#include <memory>

class Mesh;

class Parachute1 : public Body3D
{
	double k1;
	double k2;

public:

	static const T *TYPE;

	std::shared_ptr<Mesh> outerMesh;

	Parachute1(const T *type, double mass, double radius, double height, double k1, double k2,
			  std::shared_ptr<Mesh> &outerMeshRef, std::shared_ptr<Mesh> &innerMeshRef);
	void beforePhysicsStep(double  dt) override;
	void addToContainer(Scene3DContainer *container) override;
	void removeFromContainer() override;
	void updateSkin(double t) override;
};

#endif // PARACHUTE1_H
