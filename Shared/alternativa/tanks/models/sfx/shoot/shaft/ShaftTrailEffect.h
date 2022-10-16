#ifndef SHAFTTRAILEFFECT_H
#define SHAFTTRAILEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include <memory>

class Vector3;
class Material;

class ShaftTrailEffect : public IGraphicEffect
{
public:
	virtual void init(const Vector3*, const Vector3*, double, double, std::shared_ptr<Material> &material, int) = 0;
};

#endif // SHAFTTRAILEFFECT_H
