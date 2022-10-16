#include "trail2.h"
#include "alternativa/engine3d/materials/material.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/vertex.h"
#include "alternativa/engine3d/core/wrapper.h"
#include "alternativa/engine3d/core/Sorting.h"

const Object3D::T *Trail2::TYPE = new Object3D::T("Trail2");

Trail2::Trail2(const TT &t) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	this->a = this->createVertex(-1, 0, 0, 0, 0.5);
	this->b = this->createVertex(1, 0, 0, 1, 0.5);
	this->c = this->createVertex(1, 1, 0, 1, 1);
	this->d = this->createVertex(-1, 1, 0, 0, 1);
	this->face = this->createQuad(this->a, this->b, this->c, this->d);
	calculateFacesNormals();
	sorting = Sorting::DYNAMIC_BSP;
	shadowMapAlphaThreshold = 2;
	depthMapAlphaThreshold = 2;
	useShadowMap = false;
	useLight = false;
}

void Trail2::init(double param1, double param2, double param3, std::shared_ptr<Material> &material)
{
	alpha = 1;
	double _loc5_ = param1 / 2.;
	bound.MinX = this->a->x = this->d->x = -_loc5_;
	bound.MaxX = this->b->x = this->c->x = _loc5_;
	bound.MinY = this->c->y = this->d->y = param2;
	bound.MaxY = 0;
	bound.MinZ = bound.MaxZ = 0;
	this->a->v = this->b->v = param3;
	this->face->material = material;
}

void Trail2::setWidth(double value)
{
	double v = value / 2;
	bound.MinX = this->a->x = this->d->x = -v;
	bound.MaxX = this->b->x = this->c->x = v;
}

double Trail2::getLength()
{
	return this->d->y;
}

void Trail2::setLength(double value)
{
	if (value < 10)
		value = 10;

	bound.MaxY = this->c->y = this->d->y = value;
}

std::shared_ptr<Vertex> Trail2::createVertex(double x, double y, double z, double u, double v)
{
	std::shared_ptr<Vertex> vertex = Vertex::create();
	vertex->next = vertexList;
	vertexList = vertex;
	vertex->x = x;
	vertex->y = y;
	vertex->z = z;
	vertex->u = u;
	vertex->v = v;
	return vertex;
}

std::shared_ptr<Face> Trail2::createQuad(std::shared_ptr<Vertex> v1, std::shared_ptr<Vertex> v2, std::shared_ptr<Vertex> v3, std::shared_ptr<Vertex> v4)
{
	std::shared_ptr<Face> face = Face::create();
	face->next = faceList();
	setFaceList(face, 0);
	face->wrapper = Wrapper::create(61);
	face->wrapper->vertex = v1;
	face->wrapper->setNext(Wrapper::create(62));
	face->wrapper->_next->vertex = v2;
	face->wrapper->_next->setNext(Wrapper::create(63));
	face->wrapper->_next->_next->vertex = v3;
	face->wrapper->_next->_next->setNext(Wrapper::create(64));
	face->wrapper->_next->_next->_next->vertex = v4;
	return face;
}


std::shared_ptr<Trail2> new_Trail2()
{
	return std::make_shared<Trail2>(Object3D::TT(Trail2::TYPE));
}


