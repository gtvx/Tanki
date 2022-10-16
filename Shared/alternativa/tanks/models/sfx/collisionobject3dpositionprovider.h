#ifndef COLLISIONOBJECT3DPOSITIONPROVIDER_H
#define COLLISIONOBJECT3DPOSITIONPROVIDER_H

#include "alternativa/tanks/sfx/Object3DPositionProvider.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"

class TanksCollisionDetector;
class Vector3;

class CollisionObject3DPositionProvider : public Object3DPositionProvider, public PooledObject
{
	double maxDistance;
	TanksCollisionDetector *collisionDetector;
	Vector3 *localMuzzlePosition;
	Vector3 *barrelOrigin;
	Vector3 *direction;

	Object3D *turret;
	double coeff;
	double currentDistance;

public:
	CollisionObject3DPositionProvider(ObjectPool *objectPool);


	void init(Object3D *turret,
			  const Vector3 *localMuzzlePosition,
			  TanksCollisionDetector *collisionDetector,
			  double maxDistance,
			  double coeff = 0.5);

	void initPosition(Object3D *object);
	void updateObjectPosition(Object3D *object, GameCamera *camera, int value);
	void destroy();

private:
	void calculateParameters();
};

#endif // COLLISIONOBJECT3DPOSITIONPROVIDER_H
