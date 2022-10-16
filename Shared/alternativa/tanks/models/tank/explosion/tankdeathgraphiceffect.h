#ifndef TANKDEATHGRAPHICEFFECT_H
#define TANKDEATHGRAPHICEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"

class TankData;

class TankDeathGraphicEffect : public IGraphicEffect, public PooledObject
{
	TankData *tankData;
	int time;
	bool killed;
public:

	TankDeathGraphicEffect(ObjectPool*);
	void init(TankData *tankData);
	void destroy() override;
	ClientObjectUser* getOwner() override;
	bool play(int millis, GameCamera *camera) override;
	void kill() override;
	void addToContainer(Scene3DContainer *container) override;
	void createEffects();
	virtual ~TankDeathGraphicEffect();
};

#endif // TANKDEATHGRAPHICEFFECT_H
