#ifndef STATICOBJECT3DPOSITIONPROVIDER_H
#define STATICOBJECT3DPOSITIONPROVIDER_H

#include "Object3DPositionProvider.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"


class StaticObject3DPositionProvider : public Object3DPositionProvider, public PooledObject
{
	Vector3 position;
	double offsetToCamera;
public:

	StaticObject3DPositionProvider(ObjectPool *objectPool);
	void init(const Vector3 *position, double offsetToCamera);

	void initPosition(Object3D *object) override;
	void updateObjectPosition(Object3D *object, GameCamera *gameCamera, int) override;
	void destroy() override;
};

#endif // STATICOBJECT3DPOSITIONPROVIDER_H
