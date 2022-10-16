#include "animatedsprite3d.h"
#include "uvframe.h"
#include "textureanimation.h"
#include "mymath.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "__global.h"

const Object3D::T *AnimatedSprite3D::TYPE = new Object3D::T("AnimatedSprite3D");


AnimatedSprite3D::AnimatedSprite3D(const TT &t, double width, double height, std::shared_ptr<Material> material) :
	Sprite3D(TT(t.type, this->TYPE), width, height, material),
	__type_next(t.type_next)
{
	this->useShadowMap = false;
	this->useLight = false;
	this->softAttenuation = 40;

	this->currentFrame = 0;
	this->looped = false;
	this->numFrames = 0;
	this->fps = 0;
}

void AnimatedSprite3D::setAnimationData(TextureAnimation *textureAnimation)
{
	this->material = textureAnimation->material;
	this->uvFrames = textureAnimation->frames;
	this->fps = textureAnimation->fps;
	this->numFrames = this->uvFrames.length();
	this->currentFrame = 0;
	this->setFrameIndex(this->currentFrame);
}

void AnimatedSprite3D::clear()
{
	this->uvFrames.clear();;
	this->material = nullptr;
	this->numFrames = 0;
}

void AnimatedSprite3D::setFrameIndex(int v)
{
	int index = v % this->numFrames;
	this->setFrame(this->uvFrames.at(index));
}

void AnimatedSprite3D::setFrame(UVFrame *uvframe)
{
	if (uvframe == nullptr)
		return;

	topLeftU = uvframe->topLeftU;
	topLeftV = uvframe->topLeftV;
	bottomRightU = uvframe->bottomRightU;
	bottomRightV = uvframe->bottomRightV;
}

void AnimatedSprite3D::update(double v)
{
#ifdef GRAPHICS
	this->currentFrame = this->currentFrame + (this->fps * v);
	if (this->currentFrame >= this->numFrames)
	{
		if (this->looped)
		{
			this->currentFrame = MyMath::fmod(this->currentFrame, this->numFrames);
		}
		else
		{
			this->currentFrame = this->numFrames - 1;
		}
	}
	this->setFrame(this->uvFrames[int(this->currentFrame)]);
#else
	(void)v;
#endif
}


std::shared_ptr<AnimatedSprite3D> new_AnimatedSprite3D(double width, double height, std::shared_ptr<Material> material)
{
	return std::make_shared<AnimatedSprite3D>(Object3D::TT(AnimatedSprite3D::TYPE), width, height, material);
}

