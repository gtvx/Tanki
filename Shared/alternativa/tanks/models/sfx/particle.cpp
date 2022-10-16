#include "particle.h"
#include "alternativa/tanks/models/sfx/colortransform/colortransformentry.h"
#include "flash/geom/colortransform.h"

//static int INITIAL_POOL_SIZE = 20;
//static Vector.<Particle> pool = new Vector.<Particle>(INITIAL_POOL_SIZE);
//static int poolIndex = -1;

const Object3D::T *Particle::TYPE = new Object3D::T("Particle");



Particle::Particle(const TT &t, TextureAnimation *animData) :
	AnimatedSprite3D(TT(t.type, this->TYPE), 100, 100),
	__type_next(t.type_next)
{
	this->distance = 0;
	this->currFrame = 0;
	this->velocity.reset();
	this->colorTransform = std::make_shared<ColorTransform>();
	this->softAttenuation = 140;
	if (animData != nullptr)
	{
		this->setAnimationData(animData);
		this->setFrameIndex(0);
	}
	this->looped = true;
}


void Particle::dispose()
{
	//(alternativa3d::removeFromParent());
	material = nullptr;
	//int i = ++poolIndex;
	//pool[i] = this;
}

void Particle::updateColorTransofrm(double maxDistance, QVector<ColorTransformEntry*> &points)
{
	if (points.isEmpty())
	{
		return;
	}

	double t = this->distance / maxDistance;
	if (t <= 0)
	{
		const ColorTransformEntry *point1 = points[0];
		this->copyStructToColorTransform(point1, colorTransform.get());
	}
	else
	{
		if (t >= 1)
		{
			const ColorTransformEntry *point1 = points.last();
			this->copyStructToColorTransform(point1, colorTransform.get());
		}
		else
		{
			int i = 1;
			ColorTransformEntry const *point1 = points[0];
			ColorTransformEntry const *point2 = points[1];
			while (point2->t < t)
			{
				i++;
				point1 = point2;
				point2 = points[i];
			}
			t = (t - point1->t) / (point2->t - point1->t);
			this->interpolateColorTransform(point1, point2, t, colorTransform.get());
		}
	}
	alpha = colorTransform->alphaMultiplier;
}

void Particle::interpolateColorTransform(const ColorTransformEntry *ct1, const ColorTransformEntry *ct2, double t, ColorTransform *result)
{
	result->alphaMultiplier = ct1->alphaMultiplier + (t * (ct2->alphaMultiplier - ct1->alphaMultiplier));
	result->alphaOffset = ct1->alphaOffset + (t * (ct2->alphaOffset - ct1->alphaOffset));
	result->redMultiplier = ct1->redMultiplier + (t * (ct2->redMultiplier - ct1->redMultiplier));
	result->redOffset = ct1->redOffset + (t * (ct2->redOffset - ct1->redOffset));
	result->greenMultiplier = ct1->greenMultiplier + (t * (ct2->greenMultiplier - ct1->greenMultiplier));
	result->greenOffset = ct1->greenOffset + (t * (ct2->greenOffset - ct1->greenOffset));
	result->blueMultiplier = ct1->blueMultiplier + (t * (ct2->blueMultiplier - ct1->blueMultiplier));
	result->blueOffset = ct1->blueOffset + (t * (ct2->blueOffset - ct1->blueOffset));
}

void Particle::copyStructToColorTransform(const ColorTransformEntry *source, ColorTransform *result)
{
	result->alphaMultiplier = source->alphaMultiplier;
	result->alphaOffset = source->alphaOffset;
	result->redMultiplier = source->redMultiplier;
	result->redOffset = source->redOffset;
	result->greenMultiplier = source->greenMultiplier;
	result->greenOffset = source->greenOffset;
	result->blueMultiplier = source->blueMultiplier;
	result->blueOffset = source->blueOffset;
}






std::shared_ptr<Particle> new_Particle(TextureAnimation *animData)
{
	return std::make_shared<Particle>(Object3D::TT(Particle::TYPE), animData);
}

