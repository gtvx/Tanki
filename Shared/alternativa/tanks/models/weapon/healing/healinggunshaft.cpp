#include "healinggunshaft.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/wrapper.h"
#include "alternativa/engine3d/core/vertex.h"
#include "alternativa/engine3d/core/Sorting.h"

const Object3D::T *HealingGunShaft::TYPE = new Object3D::T("HealingGunShaft");


HealingGunShaft::HealingGunShaft(const TT &t) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	this->a = this->createVertex(-1, 0, 0, 0, 1);
	this->b = this->createVertex(1, 0, 0, 1, 1);
	this->c = this->createVertex(1, 1, 0, 1, 0);
	this->d = this->createVertex(-1, 1, 0, 0, 0);
	this->face = this->createQuad(this->a, this->b, this->c, this->d);
	calculateFacesNormals();
	sorting = Sorting::DYNAMIC_BSP;
	shadowMapAlphaThreshold = 2;
	depthMapAlphaThreshold = 2;
	useShadowMap = false;
	useLight = false;
}

void HealingGunShaft::setMaterial(std::shared_ptr<Material> &value)
{
	this->face->material = value;
}

void HealingGunShaft::init(double width, double length)
{
	double hw = width / 2;
	bound.MinX = this->a->x = this->d->x = -hw;
	bound.MaxX = this->b->x = this->c->x = hw;
	bound.MinY = 0;
	bound.MaxY = this->d->y = this->c->y = length;
	this->a->v = this->b->v = 1;
	bound.MinZ = bound.MaxZ = 0;
}

double HealingGunShaft::getLength()
{
	return this->d->y;
}

void HealingGunShaft::setLength(double value)
{
	if (value < 10)
		value = 10;
	bound.MaxY = this->d->y = this->c->y = value;
}

std::shared_ptr<Vertex> HealingGunShaft::createVertex(double x, double y, double z, double u, double v)
{
	std::shared_ptr<Vertex> newVertex = Vertex::create();
	newVertex->next = vertexList;
	vertexList = newVertex;
	newVertex->x = x;
	newVertex->y = y;
	newVertex->z = z;
	newVertex->u = u;
	newVertex->v = v;
	return newVertex;
}


std::shared_ptr<Face> HealingGunShaft::createQuad(std::shared_ptr<Vertex> &a,
												  std::shared_ptr<Vertex> &b,
												  std::shared_ptr<Vertex> &c,
												  std::shared_ptr<Vertex> &d)
{
	std::shared_ptr<Face> newFace = Face::create();
	newFace->next = faceList();
	setFaceList(newFace, 0);
	newFace->wrapper = Wrapper::create(12);
	newFace->wrapper->vertex = a;
	newFace->wrapper->setNext(Wrapper::create(13));
	newFace->wrapper->_next->vertex = b;
	newFace->wrapper->_next->setNext(Wrapper::create(14));
	newFace->wrapper->_next->_next->vertex = c;
	newFace->wrapper->_next->_next->setNext(Wrapper::create(15));
	newFace->wrapper->_next->_next->_next->vertex = d;
	return newFace;
}

void HealingGunShaft::destroy()
{
	this->a = nullptr;
	this->b = nullptr;
	this->c = nullptr;
	this->d = nullptr;
	this->face->destroy();
	this->face = nullptr;

	if (vertexList != nullptr)
	{
		while (vertexList->next != nullptr)
		{
			std::shared_ptr<Vertex> buf = vertexList;
			vertexList = nullptr;
			vertexList = buf->next;
		}
		vertexList = nullptr;
	}

	std::shared_ptr<Face> f = faceList();

	if (f != nullptr)
	{
		while (f->next != nullptr)
		{
			std::shared_ptr<Face> buf1 = f;
			f->wrapper->vertex = nullptr;
			f->wrapper = nullptr;
			f = nullptr;
			f = buf1->next;
		}
		setFaceList(nullptr, 0);
	}

	//captureListeners = nullptr;
	//bubbleListeners = nullptr;
}

std::shared_ptr<HealingGunShaft> new_HealingGunShaft()
{
	return std::make_shared<HealingGunShaft>(Object3D::TT(HealingGunShaft::TYPE));
}
