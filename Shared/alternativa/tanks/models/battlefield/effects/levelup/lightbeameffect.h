#ifndef LIGHTBEAMEFFECT_H
#define LIGHTBEAMEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class Object3D;
class TextureMaterial;
class LightBeam;


class LightBeamEffect : public IGraphicEffect, public PooledObject
{
	int delay;
	int time;
	double height;
	double maxHeight;
	double heightSpeed;
	double fadeSpeed;
	double x;
	double y;
	double z;
	Object3D *turret;
	Scene3DContainer *container;
	std::shared_ptr<LightBeam> beam;

public:
	LightBeamEffect(ObjectPool *objectPool);

	void init(int delay,
			  double param2,
			  double height,
			  double maxHeight,
			  double heightSpeed,
			  double fadeSpeed,
			  double x,
			  double y,
			  double z,
			  Object3D *turret,
			  TextureMaterial *material);

	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	ClientObjectUser* getOwner() override;
	void destroy() override;
	void kill() override;
};

#endif // LIGHTBEAMEFFECT_H
