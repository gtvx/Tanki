#include "animatedplane.h"

const Object3D::T *AnimatedPlane::TYPE = new Object3D::T("AnimatedPlane");


AnimatedPlane::AnimatedPlane(const TT &t, double width, double length) :
	SimplePlane(TT(t.type, this->TYPE), width, length, 0.5, 0),
	__type_next(t.type_next)
{
	this->setUVs(0, 0, 0, 1, 1, 1, 1, 0);
	shadowMapAlphaThreshold = 2;
	depthMapAlphaThreshold = 2;
}

void AnimatedPlane::setMaterials(QVector<std::shared_ptr<Material>> *materials)
{
	this->materials = materials;
	this->numFrames = materials->length();
}

void AnimatedPlane::clearMaterials()
{
	this->materials = nullptr;
	//material = nullptr;
}

void AnimatedPlane::update(double dt, double fps)
{
	this->currFrame = this->currFrame + (dt * fps);
	if (this->currFrame >= this->numFrames)
	{
		this->currFrame = 0;
	}
	//material = this->materials[int(this->currFrame)];
}

std::shared_ptr<AnimatedPlane> new_AnimatedPlane(double width, double length)
{
	return std::make_shared<AnimatedPlane>(Object3D::TT(AnimatedPlane::TYPE), width, length);
}

