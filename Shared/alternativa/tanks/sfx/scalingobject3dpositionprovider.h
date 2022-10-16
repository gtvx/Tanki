#ifndef SCALINGOBJECT3DPOSITIONPROVIDER_H
#define SCALINGOBJECT3DPOSITIONPROVIDER_H

#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "Object3DPositionProvider.h"

class Vector3;

class ScalingObject3DPositionProvider : public Object3DPositionProvider, public PooledObject
{
	Vector3 *initialPosition;
	Vector3 *velocity;
	double scaleVelocity;
public:
	ScalingObject3DPositionProvider(ObjectPool *objectPool);
	~ScalingObject3DPositionProvider();

	void init(const Vector3 *pos, const Vector3 *velocity, double scaleVelocity);

	void initPosition(Object3D *object) override;
	void updateObjectPosition(Object3D *object, GameCamera *camera, int param3) override;
	void destroy() override;
};

#endif // SCALINGOBJECT3DPOSITIONPROVIDER_H
