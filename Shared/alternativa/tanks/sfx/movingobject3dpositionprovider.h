#ifndef MOVINGOBJECT3DPOSITIONPROVIDER_H
#define MOVINGOBJECT3DPOSITIONPROVIDER_H

#include "Object3DPositionProvider.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"

class MovingObject3DPositionProvider : public Object3DPositionProvider, public PooledObject
{
	Vector3 initialPosition;
	Vector3 velocity;
	double acceleration;
public:
	MovingObject3DPositionProvider(ObjectPool*);

	void initPosition(Object3D *o) override;
	void init(const Vector3 *initialPosition, const Vector3 *velocity, double acceleration);
	void updateObjectPosition(Object3D *o, GameCamera *camera, int param3) override;
	void destroy() override;
};

#endif // MOVINGOBJECT3DPOSITIONPROVIDER_H
