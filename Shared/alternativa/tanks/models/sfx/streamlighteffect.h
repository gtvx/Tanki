#ifndef STREAMLIGHTEFFECT_H
#define STREAMLIGHTEFFECT_H

#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/tanks/sfx/IGraphicEffect.h"

class Light3D;
class LightAnimation;
class Object3DPositionProvider;


class StreamLightEffect : public PooledObject, public IGraphicEffect
{
protected:
	Light3D *light;
	LightAnimation *startAnimation;
	LightAnimation *loopAnimation;
	int startTime;
	int loopTime;
	int currentLoopTime;
	int currentTime;
	bool starting;
	Object3DPositionProvider *positionProvider;
	bool alive;
	int random;
	bool fading;
	int fadeTime;
	Scene3DContainer *container;
public:
	StreamLightEffect(ObjectPool *objectPool);

	void init(Light3D *light);

	void addToContainer(Scene3DContainer *container) override;

private:
	void startLoop();

public:
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;
	void stop();
	ClientObjectUser* getOwner() override;
};

#endif // STREAMLIGHTEFFECT_H
