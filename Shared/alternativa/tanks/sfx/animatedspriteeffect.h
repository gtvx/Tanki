#ifndef ANIMATEDSPRITEEFFECT_H
#define ANIMATEDSPRITEEFFECT_H

#include "IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/math/vector3.h"
#include <QVector>
#include <memory>

class Sprite3D;
class Material;
class ColorTransform;


class AnimatedSpriteEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<Sprite3D> sprite;
	QVector<std::shared_ptr<Material>> *materials;
	Vector3 position;
	double offsetToCamera;
	double framesPerMillisecond;
	double currFrame;
	int numFrames;
	bool loop;

public:
	AnimatedSpriteEffect(ObjectPool*);

	void init(double width,
			  double height,
			  QVector<std::shared_ptr<Material>> *materials,
			  const Vector3 *position,
			  double rotation,
			  double offsetToCamera,
			  double fps,
			  bool loop,
			  double originX = 0.5,
			  double originY = 0.5,
			  std::shared_ptr<ColorTransform> colorTransform = nullptr);

	void addToContainer(Scene3DContainer *container) override;
	ClientObjectUser* getOwner() override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;

private:
	void initSprite(double width, double height, double rotation, double originX, double originY, std::shared_ptr<ColorTransform> colorTransform);
};

#endif // ANIMATEDSPRITEEFFECT_H
