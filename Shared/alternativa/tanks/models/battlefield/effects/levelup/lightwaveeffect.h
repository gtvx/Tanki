#ifndef LIGHTWAVEEFFECT_H
#define LIGHTWAVEEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class Sprite3D;
class Object3D;
class TextureMaterial;


class LightWaveEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<Sprite3D> wave;
	Object3D *turret;
	Scene3DContainer *container;
	double maxScale;
	int delay;
	int time;
	int state;
	bool direction;

public:
	LightWaveEffect(ObjectPool *objectPool);

	void init(int delay,
			  double size,
			  double maxScale,
			  bool direction,
			  Object3D *turret,
			  TextureMaterial *material);

	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	ClientObjectUser* getOwner() override;
	void destroy() override;
	void kill() override;
};

#endif // LIGHTWAVEEFFECT_H
