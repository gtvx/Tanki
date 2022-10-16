#ifndef IGRAPHICEFFECT_H
#define IGRAPHICEFFECT_H

#include "ISpecialEffect.h"

class Scene3DContainer;

class IGraphicEffect : public ISpecialEffect
{
public:
	virtual void addToContainer(Scene3DContainer*) = 0;
};

#endif // IGRAPHICEFFECT_H
