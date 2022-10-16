#ifndef DAMAGEUPEFFECT_H
#define DAMAGEUPEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class Object3D;
class Sprite3D;
class TextureMaterial;
class QString;
enum class BlendMode;



class DamageUpEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<Sprite3D> damage;
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
	double variationX;
	double variationY;

public:
	DamageUpEffect(ObjectPool*);

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
			  std::shared_ptr<TextureMaterial> material,
			  BlendMode blendMode);

	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	ClientObjectUser* getOwner() override;
	void destroy() override;
	void kill() override;
};

#endif // DAMAGEUPEFFECT_H
