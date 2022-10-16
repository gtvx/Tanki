#ifndef LIMITEDDISTANCEANIMATEDSPRITEEFFECT_H
#define LIMITEDDISTANCEANIMATEDSPRITEEFFECT_H

#include "IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "flash/display/BlendMode.h"
#include <memory>

class Object3DPositionProvider;
class AnimatedSprite3D;
enum class BlendMode;
class ColorTransform;
class TextureAnimation;



class LimitedDistanceAnimatedSpriteEffect : public IGraphicEffect, public PooledObject
{
	Object3DPositionProvider *positionProvider;
	Scene3DContainer *container;
	double currentFrame;
	double framesPerMs;
	double fallOffStart;
	double distanceToDisable;
	double alphaMultiplier;
	int loopsCount;

public:
	std::shared_ptr<AnimatedSprite3D> sprite;

	LimitedDistanceAnimatedSpriteEffect(ObjectPool *objectPool);

	void init(double width,
			  double height,
			  TextureAnimation *textureAnimation,
			  double rotation,
			  Object3DPositionProvider *positionProvider,
			  double originX = 0.5,
			  double originY = 0.5,
			  std::shared_ptr<ColorTransform> param8 = nullptr,
			  double softAttenuation = 130,
			  BlendMode blendMode = BlendMode::NORMAL,
			  double fallOffStart = 1000000,
			  double distanceToDisable = 1000000,
			  double alphaMultiplier = 1,
			  bool shadowMapLight = false);

	void addToContainer(Scene3DContainer *container) override;

	bool play(int millis, GameCamera *camera) override;

	void destroy() override;

	void kill() override;

private:
	void initSprite(double width,
					double height,
					double rotation,
					double originX,
					double originY,
					std::shared_ptr<ColorTransform> &colorTransform,
					TextureAnimation *textureAnimation,
					double softAttenuation,
					BlendMode blendMode);

	ClientObjectUser* getOwner() override;
};

#endif // LIMITEDDISTANCEANIMATEDSPRITEEFFECT_H
