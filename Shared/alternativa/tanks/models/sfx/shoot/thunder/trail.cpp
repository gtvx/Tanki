#include "trail.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/vertex.h"
#include "alternativa/engine3d/core/wrapper.h"


const Object3D::T *Trail::TYPE = new Object3D::T("Trail");


Trail::Trail(const TT &t, double scale, std::shared_ptr<Material> material) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	double w = 4;
	double h = 240 * scale;
	std::shared_ptr<Vertex> a = this->createVertex(-w, 0, 0, 0, 0);
	std::shared_ptr<Vertex> b = this->createVertex(w, 0, 0, 0, 1);
	std::shared_ptr<Vertex> c = this->createVertex(0, h, 0, 1, 0.5);
	this->createFace(a, b, c)->material = material;
	this->createFace(c, b, a)->material = material;
	calculateFacesNormals(true);
	calculateBounds();
	blendMode = BlendMode::SCREEN;
	alpha = 0.3;
}

std::shared_ptr<Vertex> Trail::createVertex(double x, double y, double z, double u, double v)
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

std::shared_ptr<Face> Trail::createFace(std::shared_ptr<Vertex> &a, std::shared_ptr<Vertex> &b, std::shared_ptr<Vertex> &c)
{
	std::shared_ptr<Face> newFace = Face::create();
	newFace->next = faceList();
	setFaceList(newFace, 30);
	newFace->wrapper = Wrapper::create(65);
	newFace->wrapper->vertex = a;
	newFace->wrapper->setNext(Wrapper::create(66));
	newFace->wrapper->_next->vertex = b;
	newFace->wrapper->_next->setNext(Wrapper::create(67));
	newFace->wrapper->_next->_next->vertex = c;
	return newFace;
}

std::shared_ptr<Trail> new_Trail(double scale, std::shared_ptr<Material> material)
{
	return std::make_shared<Trail>(Object3D::TT(Trail::TYPE), scale, material);
}
