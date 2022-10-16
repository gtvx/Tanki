#ifndef ANIMATEDLIGHTEFFECT_H
#define ANIMATEDLIGHTEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class Vector3;
class OmniLight;
class Object3DPositionProvider;
class LightAnimation;



class AnimatedLightEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<OmniLight> light;
	Object3DPositionProvider *positionProvider;
	LightAnimation *animation;
	int currentTime;
	int timeToLive;
	bool looped;
	bool alive;
	double maxDistance;
	double fadeDistance;
	Vector3 *position;
	Scene3DContainer *container;
public:
	AnimatedLightEffect(ObjectPool *objectPool);

	void init(Object3DPositionProvider *positionProvider, LightAnimation *animation, double maxDistance = 99999, bool looped = false);
	void initFromTime(Object3DPositionProvider *positionProvider, int timeToLive, LightAnimation *animation, double maxDistance = 99999, bool looped = false);
	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;
	ClientObjectUser* getOwner() override;
};

#endif // ANIMATEDLIGHTEFFECT_H
