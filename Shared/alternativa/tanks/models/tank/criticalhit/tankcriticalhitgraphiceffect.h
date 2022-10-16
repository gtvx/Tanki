#ifndef TANKCRITICALHITGRAPHICEFFECT_H
#define TANKCRITICALHITGRAPHICEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"

class TankData;

class TankCriticalHitGraphicEffect : public IGraphicEffect
{
	TankData *tankData;
	int time;
	bool killed;

public:
	TankCriticalHitGraphicEffect(TankData *tankData);

	void destroy() override;
	ClientObjectUser* getOwner() override;
	bool play(int millis, GameCamera *camera) override;
	void kill() override;
	void addToContainer(Scene3DContainer *container) override;
	void createEffects();
};

#endif // TANKCRITICALHITGRAPHICEFFECT_H
