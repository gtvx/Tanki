#ifndef SPARKEFFECT_H
#define SPARKEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class Object3D;
class TextureMaterial;
enum class BlendMode;
class Sprite3D;


class SparkEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<Sprite3D> spark;
	int delay;
	int time;
	double maxHeight;
	double visibleHeight;
	double heightSpeed;
	double x;
	double y;
	double z;
	Object3D *object;
	Scene3DContainer *container;

public:
	SparkEffect(ObjectPool *objectPool);

	void init(int delay,
			  double width,
			  double height,
			  double rotation,
			  double maxHeight,
			  double visibleHeight,
			  double heightSpeed,
			  double x,
			  double y,
			  double z,
			  Object3D *object,
			  TextureMaterial *material,
			  BlendMode blendMode);

	void addToContainer(Scene3DContainer *container);
	bool play(int millis, GameCamera *camera);
	ClientObjectUser* getOwner();
	void destroy();
	void kill();
};

#endif // SPARKEFFECT_H
