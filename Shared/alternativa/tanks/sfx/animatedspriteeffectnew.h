#ifndef ANIMATEDSPRITEEFFECTNEW_H
#define ANIMATEDSPRITEEFFECTNEW_H

#include "IGraphicEffect.h"
#include "flash/display/BlendMode.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include <memory>

class AnimatedSprite3D;
class Object3DPositionProvider;
class ObjectPool;
class ColorTransform;
class TextureAnimation;


class AnimatedSpriteEffectNew : public IGraphicEffect,  public PooledObject
{
	std::shared_ptr<AnimatedSprite3D> sprite;
	double currentFrame;
	double framesPerMs;
	int loopsCount;
	Object3DPositionProvider *positionProvider;
	Scene3DContainer *container;


public:
	AnimatedSpriteEffectNew(ObjectPool*);

	void init(double width,
			  double height,
			  TextureAnimation *textureAnimation,
			  double rotation,
			  Object3DPositionProvider *object3DPositionProvider,
			  double originX = 0.5,
			  double originY = 0.5,
			  std::shared_ptr<ColorTransform> colorTransform = nullptr,
			  double softAttenuation = 130,
			  BlendMode blendMode = BlendMode::NORMAL);

	void addToContainer(Scene3DContainer *container) override;
	bool play(int millis, GameCamera *camera) override;
	void destroy() override;
	void kill() override;
	ClientObjectUser* getOwner() override;

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
};

#endif // ANIMATEDSPRITEEFFECTNEW_H
