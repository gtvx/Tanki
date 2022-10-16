#ifndef BEAMEFFECTS_H
#define BEAMEFFECTS_H

#include <QHash>

class ClientObjectUser;
class IGraphicEffect;
class IBattleField;

class BeamEffects
{
	QHash<ClientObjectUser*, IGraphicEffect*> effects;
	IBattleField *battleService;
public:
	BeamEffects(IBattleField *battleService);
	void addEffect(ClientObjectUser *user, IGraphicEffect *effect);
	void removeEffect(ClientObjectUser *user);
};

#endif // BEAMEFFECTS_H
