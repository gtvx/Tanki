#include "beameffects.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/sfx/IGraphicEffect.h"


BeamEffects::BeamEffects(IBattleField *battleService)
{
	this->battleService = battleService;
}

void BeamEffects::addEffect(ClientObjectUser *user, IGraphicEffect *effect)
{
	this->effects[user] = effect;
	this->battleService->addGraphicEffect(effect);
}

void BeamEffects::removeEffect(ClientObjectUser *user)
{
	IGraphicEffect *effect = this->effects.value(user);
	if (effect != nullptr)
	{
		effect->kill();
		this->effects.remove(user);
	}
}
