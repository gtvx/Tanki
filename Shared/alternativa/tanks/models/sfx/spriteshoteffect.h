#ifndef SPRITESHOTEFFECT_H
#define SPRITESHOTEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class Sprite3D;
class Object3D;
class Material;
class ColorTransform;


class SpriteShotEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<Sprite3D> sprite;
	Object3D *turret;
	Vector3 localPosition;
	int time;
	int state;
	int brightTime;
	int fadeTime;
public:

	SpriteShotEffect(ObjectPool*);

	void init(Material *material,
			  const Vector3 *localMuzzlePosition,
			  Object3D *turret,
			  double localOffset,
			  double size,
			  int brightTime,
			  int fadeTime,
			  std::shared_ptr<ColorTransform> &colorTransform);

	ClientObjectUser* getOwner() override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void addToContainer(Scene3DContainer *container) override;
	void kill() override;
};

#endif // SPRITESHOTEFFECT_H
