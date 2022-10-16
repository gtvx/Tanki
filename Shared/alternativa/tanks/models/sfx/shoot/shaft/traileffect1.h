#ifndef TRAILEFFECT1_H
#define TRAILEFFECT1_H

#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "ShaftTrailEffect.h"
#include <memory>

class Trail1;

class TrailEffect1 : public PooledObject, public ShaftTrailEffect
{
	Vector3 *position;
	Vector3 *direction;
	double alphaSpeed;
	int timeToLive;
	std::shared_ptr<Trail1> beam;
	Scene3DContainer *container;
public:

	static const double WIDTH;

	TrailEffect1(ObjectPool *objectPool);


	void init(const Vector3 *position,
			  const Vector3 *direction,
			  double param3,
			  double param4,
			  std::shared_ptr<Material> &material,
			  int timeToLive) override;

	bool play(int millis, GameCamera *camera) override;
	void addToContainer(Scene3DContainer *container) override;
	void destroy() override;
	void kill() override;
	ClientObjectUser* getOwner() override;
};

#endif // TRAILEFFECT1_H
