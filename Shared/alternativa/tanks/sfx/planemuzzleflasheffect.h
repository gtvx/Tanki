#ifndef PLANEMUZZLEFLASHEFFECT_H
#define PLANEMUZZLEFLASHEFFECT_H

#include "IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/math/vector3.h"
#include <memory>

class SimplePlane;
class Object3D;
class Vector3;
class ObjectPool;
class ClientObject;
class TextureMaterial;

class PlaneMuzzleFlashEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<SimplePlane> plane;
	Object3D *turret;
	Vector3 localMuzzlePosition;
	Scene3DContainer *container;
	int timetoLive;

public:
	PlaneMuzzleFlashEffect(ObjectPool*);
	void init(const Vector3 *pos, Object3D *turret, TextureMaterial *material, int timetoLive, double width, double length);
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;
	void addToContainer(Scene3DContainer*)  override;
	ClientObjectUser* getOwner() override;
};

#endif // PLANEMUZZLEFLASHEFFECT_H
