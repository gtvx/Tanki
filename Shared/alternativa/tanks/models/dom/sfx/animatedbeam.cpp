#include "animatedbeam.h"
#include "alternativa/engine3d/core/Sorting.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/vertex.h"
#include "alternativa/engine3d/materials/texturematerial.h"

const Object3D::T *AnimatedBeam::TYPE = new Object3D::T("AnimatedBeam");


AnimatedBeam::AnimatedBeam(const TT &t, double param1, double param2, double unitLength, double animationSpeed) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	this->unitLength = unitLength;
	this->animationSpeed = animationSpeed;
	useShadowMap = false;
	useLight = false;
	shadowMapAlphaThreshold = 2;
	depthMapAlphaThreshold = 2;
	double _loc5_ = param1 / 2.;

	const double _loc6_[36] = {-_loc5_, 0, 0, _loc5_, 0, 0, _loc5_, param2, 0, -_loc5_,
							   param2, 0, -_loc5_, param2, 0, _loc5_, param2, 0, _loc5_, param2 + 1,
							   0, -_loc5_, param2 + 1, 0, -_loc5_, param2 + 1, 0, _loc5_, param2 + 1, 0,
							   _loc5_, (2 * param2) + 1, 0, -_loc5_, (2 * param2) + 1, 0};

	const double _loc7_[24] = {0, 1, 1, 1,
							   1, 0, 0, 0,
							   0, 1, 1, 1,
							   1, 0, 0, 0,
							   0, 0, 1, 0,
							   1, 1, 0, 1};

	const int _loc8_[15] = {4, 0, 1, 2, 3, 4, 4, 5, 6, 7, 4, 8, 9, 10, 11};

	addVerticesAndFaces(_loc6_, 36, _loc7_, 24, _loc8_, 15, true);
	sorting = Sorting::DYNAMIC_BSP;
	this->writeVertices();
	calculateFacesNormals();
	this->updateV();
	bound.MinX = -_loc5_;
	bound.MaxX = _loc5_;
	bound.MinY = 0;
	bound.MaxY = 1; //length;
	bound.MinZ = 0;
	bound.MaxZ = 0;
}

void AnimatedBeam::setMaterials(TextureMaterial *t1, TextureMaterial *t2)
{
	Face *face = faceList().get();
	face->material = t1->ptr.lock();
	face->next->material = t2->ptr.lock();
	face->next->next->material = t1->ptr.lock();
}

void AnimatedBeam::writeVertices()
{
	Vertex *list[12];
	this->getVertices(list, 12);
	this->a = list[0];
	this->b = list[1];
	this->c = list[2];
	this->d = list[3];
	this->e = list[4];
	this->f = list[5];
	this->g = list[6];
	this->h = list[7];
	this->i = list[8];
	this->j = list[9];
	this->k = list[10];
	this->l = list[11];
}

void AnimatedBeam::updateV()
{
	this->e->v = this->vOffset;
	this->f->v = this->vOffset;
	double v = (((this->g->y - this->f->y) / this->unitLength) + this->vOffset);
	this->g->v = v;
	this->h->v = v;
}

void AnimatedBeam::clear()
{
	setMaterialToAllFaces(nullptr);
}

void AnimatedBeam::setTipLength(double value)
{
	double v = this->c->y;
	this->c->y = value;
	this->d->y = value;
	this->e->y = value;
	this->f->y = value;
	this->setLength((this->k->y + value) - v);
}

void AnimatedBeam::resize(double width, double length)
{
	this->setWidth(width);
	this->setLength(length);
}

void AnimatedBeam::setWidth(double width)
{
	double v = width / 2;
	bound.MinX = -v;
	bound.MaxX = v;
	this->a->x = -v;
	this->d->x = -v;
	this->e->x = -v;
	this->h->x = -v;
	this->i->x = -v;
	this->l->x = -v;
	this->b->x = v;
	this->c->x = v;
	this->f->x = v;
	this->g->x = v;
	this->j->x = v;
	this->k->x = v;
}

void AnimatedBeam::setLength(double length)
{
	if (length < (1 + (2 * this->c->y)))
	{
		visible = false;
	}
	else
	{
		visible = true;
		bound.MaxY = length;
		this->g->y = length - this->c->y;
		this->h->y = this->g->y;
		this->i->y = this->g->y;
		this->j->y = this->g->y;
		this->k->y = length;
		this->l->y = length;
		this->updateV();
	}
}

void AnimatedBeam::setURange(double value)
{
	this->a->u = 0.5 * (1 - value);
	this->d->u = this->a->u;
	this->e->u = this->a->u;
	this->h->u = this->a->u;
	this->i->u = this->a->u;
	this->l->u = this->a->u;
	this->b->u = 0.5 * (1 + value);
	this->c->u = this->b->u;
	this->f->u = this->b->u;
	this->g->u = this->b->u;
	this->j->u = this->b->u;
	this->k->u = this->b->u;
}

void AnimatedBeam::update(double v)
{
	this->vOffset = this->vOffset + (this->animationSpeed * v);
	if (this->vOffset < 0)
	{
		this->vOffset = this->vOffset + 1;
	}
	else
	{
		if (this->vOffset > 1)
		{
			this->vOffset = this->vOffset - 1;
		}
	}
	this->updateV();
}

void AnimatedBeam::setUnitLength(double value)
{
	this->unitLength = value;
	this->updateV();
}

std::shared_ptr<AnimatedBeam> new_AnimatedBeam(double param1, double param2, double unitLength, double animationSpeed)
{
	return std::make_shared<AnimatedBeam>(Object3D::TT(AnimatedBeam::TYPE), param1, param2, unitLength, animationSpeed);
}
