#ifndef BEAMEFFECT_H
#define BEAMEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class ShotTrail;
class Vector3;
class Material;


class BeamEffect : public IGraphicEffect, public PooledObject
{
	Scene3DContainer *container;
	std::shared_ptr<ShotTrail> trail;
	Vector3 *startPoint;
	Vector3 *direction;
	double beginScale;
	double endScale;
	double moveDistance;
	int lifeTime;
	int time;

public:
	BeamEffect(ObjectPool *bjectPool);

	void init(const Vector3 *param1,
			  const Vector3 *param2,
			  std::shared_ptr<Material> &material,
			  double param4,
			  double beginScale,
			  double endScale,
			  double moveDistance,
			  int lifeTime);

	bool play(int millis, GameCamera *camera) override;
	void addToContainer(Scene3DContainer *container) override;
	void destroy() override;
	void kill() override;
	ClientObjectUser* getOwner() override;
};

#endif // BEAMEFFECT_H
