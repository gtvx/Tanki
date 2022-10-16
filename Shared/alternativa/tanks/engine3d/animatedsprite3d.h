#ifndef ANIMATEDSPRITE3D_H
#define ANIMATEDSPRITE3D_H

#include "alternativa/engine3d/objects/sprite3d.h"

class UVFrame;
class TextureAnimation;

class AnimatedSprite3D : public Sprite3D
{
	QVector<UVFrame*> uvFrames;
	int numFrames;
	double fps;

public:

	static const T *TYPE;
	const T *__type_next;

	bool looped;
	double currentFrame;
	AnimatedSprite3D(const TT &t, double width, double height, std::shared_ptr<Material> material = nullptr);




	void setAnimationData(TextureAnimation *textureAnimation);

	double getFps()
	{
		return this->fps;
	}

	int getNumFrames()
	{
		return this->numFrames;
	}

	void clear();

	void setFrameIndex(int param1);

private:
	void setFrame(UVFrame *param1);

public:
	void update(double param1);

};

std::shared_ptr<AnimatedSprite3D> new_AnimatedSprite3D(double width, double height, std::shared_ptr<Material> material = nullptr);


#endif // ANIMATEDSPRITE3D_H
