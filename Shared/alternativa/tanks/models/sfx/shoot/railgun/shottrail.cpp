#include "shottrail.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/Sorting.h"
#include "alternativa/engine3d/core/vertex.h"
#include "flash/display/bitmapdata.h"


const Object3D::T *ShotTrail::TYPE = new Object3D::T("ShotTrail");



ShotTrail::ShotTrail(const TT &t) :
	Mesh(TT(t.type, this->TYPE))
{
	this->a = addVertex(-1, 1, 0);
	this->b = addVertex(-1, 0, 0);
	this->c = addVertex(1, 0, 0);
	this->d = addVertex(1, 1, 0);
	this->face = addQuadFace(this->a, this->b, this->c, this->d);
	calculateFacesNormals();
	sorting = Sorting::DYNAMIC_BSP;
	softAttenuation = 80;
	shadowMapAlphaThreshold = 2;
	depthMapAlphaThreshold = 2;
	useShadowMap = false;
	useLight = false;
}

void ShotTrail::init(double param1, double param2, std::shared_ptr<Material> &material, double param4)
{
	double _loc5_ = param1 * 0.5;
	this->a->x = -_loc5_;
	this->a->y = param2;
	this->a->u = 0;
	this->b->x = -(_loc5_);
	this->b->y = 0;
	this->b->u = 0;
	this->c->x = _loc5_;
	this->c->y = 0;
	this->c->u = 1;
	this->d->x = _loc5_;
	this->d->y = param2;
	this->d->u = 1;
	bound.MinX = -_loc5_;
	bound.MinY = 0;
	bound.MinZ = 0;
	bound.MaxX = _loc5_;
	bound.MaxY = param2;
	bound.MaxZ = 0;
	this->face->material = material;

	TextureMaterial *textureMaterial = nullptr;

	if (material->isType(TextureMaterial::TYPE))
	{
		textureMaterial = (TextureMaterial*)material.get();

		BitmapData *image = textureMaterial->getTexturePtr();
		if (image != nullptr)
		{
			this->bottomV = param2 / ((param1 * image->height()) / image->width());
			this->distanceV = param4 / param1;
			return;
		}
	}

	this->bottomV = 1;
	this->distanceV = 0;
}

void ShotTrail::clear()
{
	this->face->material = nullptr;
}

void ShotTrail::update(double param1)
{
	double v = this->distanceV * param1;
	this->a->v = v;
	this->b->v = this->bottomV + v;
	this->c->v = this->bottomV + v;
	this->d->v = v;
}



std::shared_ptr<ShotTrail> new_ShotTrail()
{
	return std::make_shared<ShotTrail>(Object3D::TT(ShotTrail::TYPE));
}
