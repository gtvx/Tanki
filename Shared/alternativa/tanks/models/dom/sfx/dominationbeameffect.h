#ifndef DOMINATIONBEAMEFFECT_H
#define DOMINATIONBEAMEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/math/vector3.h"
#include <memory>

class AnimatedBeam;
class Object3D;
class Dictionary;
class BeamProperties;


class DominationBeamEffect : public IGraphicEffect, public PooledObject
{

	std::shared_ptr<AnimatedBeam> beam;
	bool alive;
	Object3D *object;
	Vector3 endPosition;
	Dictionary *excludedObects;
	Scene3DContainer *container;

public:
	DominationBeamEffect(ObjectPool *pool);
	void init(Object3D *object, const Vector3 *pos, const BeamProperties *beamProperties, Dictionary *excludedObects);
	bool play(int millis, GameCamera *camera);
	void addToContainer(Scene3DContainer *container);
	void destroy();
	void kill();
	ClientObjectUser* getOwner();
	void setBeamWidth(double value);
};

#endif // DOMINATIONBEAMEFFECT_H
