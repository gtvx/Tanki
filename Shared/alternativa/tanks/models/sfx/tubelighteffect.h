#ifndef TUBELIGHTEFFECT_H
#define TUBELIGHTEFFECT_H

#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include <memory>

class TubeLight;
class Object3DPositionProvider;
class LightAnimation;
class Object3D;


class TubeLightEffect : public PooledObject, public IGraphicEffect
{
	std::shared_ptr<TubeLight> light;
	Object3DPositionProvider *positionProvider;
	Object3DPositionProvider *targetProvider;
	LightAnimation *animation;
	int currentTime;
	int timeToLive;
	bool looped;
	bool alive;
	std::shared_ptr<Object3D> target;
	Scene3DContainer *container;

public:
	TubeLightEffect(ObjectPool *objectPool);

	void init(Object3DPositionProvider *positionProvider,
			  Object3DPositionProvider *targetProvider,
			  LightAnimation *animation,
			  bool looped = false);

	void initFromTime(Object3DPositionProvider *positionProvider,
					  Object3DPositionProvider *targetProvider,
					  int timeToLive,
					  LightAnimation *animation,
					  bool looped = false);

	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;
	ClientObjectUser* getOwner() override;
};

#endif // TUBELIGHTEFFECT_H
