#include "cords2.h"
#include "parachute2.h"
#include "bonusmesh.h"
#include "alternativa/engine3d/core/vertex.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/wrapper.h"
#include "mymath.h"
#include "bonuscache.h"
#include <QDebug>


const Object3D::T *Cords2::TYPE = new Object3D::T("Cords2");


Cords2::Cords2(const TT &t, double radius, double boxHalfSize, int numStraps, Material *material) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	this->numStraps = numStraps;
	this->topVertices = new std::shared_ptr<Vertex>[2 * numStraps];
	this->topLocalPoints = new Vector3[numStraps];
	this->createGeometry(radius, boxHalfSize);
	setMaterialToAllFaces(material);
	shadowMapAlphaThreshold = 2;
	setName("cords");
}

Cords2::~Cords2()
{
	delete[] this->topVertices;
	delete[] this->topLocalPoints;
}

void Cords2::init(BonusMesh *bonusMesh, Parachute2 *parachute)
{
	this->bonusMesh = bonusMesh;
	this->parachute = parachute;
	scale.reset(1, 1, 1);
	alpha = 1;
}

void Cords2::updateVertices()
{
	if (this->parachute == nullptr)
	{
		return;
	}
	Matrix4 meshMatrix;

	meshMatrix.setMatrix(&this->parachute->position, &this->parachute->rotation);

	for (int i = 0; i < this->numStraps; i++)
	{
		const Vector3 *point = &this->topLocalPoints[i];
		double x = ((((point->x * meshMatrix.a) + (point->y * meshMatrix.b)) + (point->z * meshMatrix.c)) + meshMatrix.d);
		double y = ((((point->x * meshMatrix.e) + (point->y * meshMatrix.f)) + (point->z * meshMatrix.g)) + meshMatrix.h);
		double z = ((((point->x * meshMatrix.i) + (point->y * meshMatrix.j)) + (point->z * meshMatrix.k)) + meshMatrix.l);
		Vertex *v = this->topVertices[2 * i].get();
		v->x = x;
		v->y = y;
		v->z = z;
		v = this->topVertices[(2 * i) + 1].get();
		v->x = x;
		v->y = y;
		v->z = z;
	}

	meshMatrix.setMatrix(&this->bonusMesh->position, &bonusMesh->rotation);
	Vector3 *point = &this->boxLocalPoint;
	this->boxVertex->x = ((((point->x * meshMatrix.a) + (point->y * meshMatrix.b)) + (point->z * meshMatrix.c)) + meshMatrix.d);
	this->boxVertex->y = ((((point->x * meshMatrix.e) + (point->y * meshMatrix.f)) + (point->z * meshMatrix.g)) + meshMatrix.h);
	this->boxVertex->z = ((((point->x * meshMatrix.i) + (point->y * meshMatrix.j)) + (point->z * meshMatrix.k)) + meshMatrix.l);
	calculateBounds();
	calculateFacesNormals();
}

void Cords2::createGeometry(double radius, double boxHalfSize)
{
	this->boxLocalPoint.reset(0, 0, boxHalfSize);
	this->boxVertex = this->createVertex(0, 0, boxHalfSize, 0, 1);
	double angleStep = (2 * MyMath::PI) / this->numStraps;

	int i = 0;
	while (i < this->numStraps)
	{
		double angle = i * angleStep;
		Vector3 *localPoint = &this->topLocalPoints[i];
		localPoint->reset((radius * MyMath::cos(angle)), (radius * MyMath::sin(angle)), 0);
		this->topVertices[2 * i] = this->createVertex(localPoint->x, localPoint->y, localPoint->z, 0, 0);
		this->topVertices[((2 * i) + 1)] = this->createVertex(localPoint->x, localPoint->y, localPoint->z, 1, 1);
		i++;
	}

	int faceIndex = 0;
	while (faceIndex < this->numStraps)
	{
		int firstIndex = (2 * faceIndex);
		int secondIndex = (firstIndex + 3);
		if (secondIndex >= (2 * this->numStraps))
		{
			secondIndex -= (2 * this->numStraps);
		}
		this->createTriFace(this->boxVertex, this->topVertices[firstIndex], this->topVertices[secondIndex]);
		this->createTriFace(this->boxVertex, this->topVertices[secondIndex], this->topVertices[firstIndex]);
		faceIndex++;
	}
}

std::shared_ptr<Vertex> Cords2::createVertex(double x, double y, double z, double u, double v)
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

std::shared_ptr<Face> Cords2::createTriFace(std::shared_ptr<Vertex> a, std::shared_ptr<Vertex> b, std::shared_ptr<Vertex> c)
{
	std::shared_ptr<Face> newFace = Face::create();
	newFace->next = faceList();
	setFaceList(newFace, 0);
	newFace->wrapper = Wrapper::create(0);
	newFace->wrapper->vertex = a;
	newFace->wrapper->setNext(Wrapper::create(0));
	newFace->wrapper->next()->vertex = b;
	newFace->wrapper->next()->setNext(Wrapper::create(0));
	newFace->wrapper->next()->next()->vertex = c;
	return newFace;
}

void Cords2::recycle()
{
	this->bonusMesh = nullptr;
	this->parachute = nullptr;
	BonusCache::putCords(std::dynamic_pointer_cast<Cords2>(this->ptr()));
}

void Cords2::setAlpha(double value)
{
	this->ownAlpha = value;
	this->updateAlpha();
}

void Cords2::updateAlpha()
{
	alpha = this->alphaMultiplier * this->ownAlpha;
}

void Cords2::readPosition(Vector3 *result)
{
	result->copy(&this->position);
}

void Cords2::setAlphaMultiplier(double value)
{
	this->alphaMultiplier = value;
	this->updateAlpha();
}


std::shared_ptr<Cords2> new_Cords2(double radius, double boxHalfSize, int numStraps, Material *material)
{
	return std::make_shared<Cords2>(Object3D::TT(Cords2::TYPE), radius, boxHalfSize, numStraps, material);
}
