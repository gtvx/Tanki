#ifndef MUZZLEPOSITIONPROVIDER_H
#define MUZZLEPOSITIONPROVIDER_H

#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/tanks/sfx/Object3DPositionProvider.h"

class Object3D;
class Vector3;
class GameCamera;


class MuzzlePositionProvider : public PooledObject, public Object3DPositionProvider
{
	Object3D *turret;
	Vector3 *localPosition;
public:
	MuzzlePositionProvider(ObjectPool *objectPool);
	virtual ~MuzzlePositionProvider();

	void init(Object3D *turret, const Vector3 *pos, double offset_y = 0);
	void initPosition(Object3D*) override;
	void updateObjectPosition(Object3D *o, GameCamera *camera, int v) override;
	void destroy() override;
};

#endif // MUZZLEPOSITIONPROVIDER_H
