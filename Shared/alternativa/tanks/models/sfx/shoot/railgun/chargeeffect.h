#ifndef CHARGEEFFECT_H
#define CHARGEEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>
#include <QVector>


class AnimSprite;
class Material;
class Vector3;
class Object3D;
class ColorTransform;


class ChargeEffect : public IGraphicEffect, public PooledObject
{
	std::shared_ptr<AnimSprite> sprite;
	ClientObjectUser *_owner;
	double framesPerMillisecond;
	double currFrame;
	QVector<std::shared_ptr<Material>> *materials;
	int numFrames;
	Vector3 *localMuzzlePosition;
	Object3D *turret;
public:
	ChargeEffect(ObjectPool *objectPool);

	void init(ClientObjectUser *owner,
			  double width,
			  double height,
			  QVector<std::shared_ptr<Material>> *materials,
			  const Vector3 *localMuzzlePosition,
			  Object3D *turret,
			  double rotation,
			  double fps,
			  std::shared_ptr<ColorTransform> &colorTransform);

	void addToContainer(Scene3DContainer *container) override;
	ClientObjectUser* getOwner() override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;

private:
	void initSprite(double width,
					double height,
					double rotation,
					std::shared_ptr<ColorTransform> &colorTransform);
};

#endif // CHARGEEFFECT_H
