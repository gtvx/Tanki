#ifndef ANIMATEDPLANEEFFECTOLD_H
#define ANIMATEDPLANEEFFECTOLD_H

#include <QVector>
#include <memory>
#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"

class Material;
class Vector3;
class Plane;


class AnimatedPlaneEffectOld : public IGraphicEffect, public PooledObject
{
	int numFrames;
	double framesPerMillisecond;
	double scaleSpeed;
	double currentFrame;
	double scale;
	double baseScale;
	std::shared_ptr<Plane> plane;
	QVector<std::shared_ptr<Material>> *frames;

public:
	AnimatedPlaneEffectOld(ObjectPool*);

	void init(double size, const Vector3 *position, const Vector3 *rotation, double fps, QVector<std::shared_ptr<Material>> *frames, double scaleSpeed);
	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;
	ClientObjectUser* getOwner() override;
};

#endif // ANIMATEDPLANEEFFECTOLD_H
