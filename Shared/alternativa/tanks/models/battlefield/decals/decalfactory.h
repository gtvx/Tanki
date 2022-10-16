#ifndef DECALFACTORY_H
#define DECALFACTORY_H

#include "RotationState.h"
#include <memory>

class ICollisionDetector;
class Decal;
class Vector3;
class TextureMaterial;
class KDContainer;


class DecalFactory
{
	ICollisionDetector *collisionDetector;
public:

	DecalFactory(ICollisionDetector *collisionDetector);

	std::shared_ptr<Decal> createDecal(const Vector3 *hitPosition,
									   const Vector3 *muzzlePosition,
									   double radius,
									   TextureMaterial *material,
									   KDContainer *container,
									   RotationState rotationState);
};

#endif // DECALFACTORY_H
