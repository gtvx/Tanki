#include "cords1.h"
#include "alternativa/engine3d/core/vertex.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/wrapper.h"
#include "mymath.h"


const Object3D::T *Cords1::TYPE = new Object3D::T("Cords1");


Cords1::Cords1(const TT &t, double radius, double boxHalfSize, int numStraps, Object3D *box, Object3D *parachute) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	this->box = box;
	this->parachute = parachute;
	this->numStraps = numStraps;
	this->topVertices = new std::shared_ptr<Vertex>[2 * numStraps];
	this->topLocalPoints = new Vector3[numStraps];
	this->createGeometry(radius, boxHalfSize);
}

Cords1::~Cords1()
{
	delete[] this->topVertices;
	delete[] this->topLocalPoints;
}

void Cords1::updateVertices()
{
	Matrix4 meshMatrix;

	meshMatrix.setMatrix(&this->parachute->position, &this->parachute->rotation);
	int i = 0;
	while (i < this->numStraps)
	{
		Vector3 *point = &this->topLocalPoints[i];
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
		i++;
	}
	meshMatrix.setMatrix(&this->box->position, &this->box->rotation);
	const Vector3 *point = &this->boxLocalPoint;
	this->boxVertex->x = ((((point->x * meshMatrix.a) + (point->y * meshMatrix.b)) + (point->z * meshMatrix.c)) + meshMatrix.d);
	this->boxVertex->y = ((((point->x * meshMatrix.e) + (point->y * meshMatrix.f)) + (point->z * meshMatrix.g)) + meshMatrix.h);
	this->boxVertex->z = ((((point->x * meshMatrix.i) + (point->y * meshMatrix.j)) + (point->z * meshMatrix.k)) + meshMatrix.l);
	calculateBounds();
	calculateFacesNormals();
}

void Cords1::createGeometry(double radius, double boxHalfSize)
{
	this->boxLocalPoint.reset(0, 0, boxHalfSize);
	this->boxVertex = this->createVertex(0, 0, boxHalfSize, 0, 1);
	double angleStep = (2 * MyMath::PI) / this->numStraps;
	int i = 0;
	while (i < this->numStraps)
	{
		double angle = i * angleStep;
		Vector3 *localPoint = &this->topLocalPoints[i];
		localPoint->reset(radius * MyMath::cos(angle), radius * MyMath::sin(angle), 0);
		this->topVertices[2 * i] = this->createVertex(localPoint->x, localPoint->y, localPoint->z, 0, 0);
		this->topVertices[(2 * i) + 1] = this->createVertex(localPoint->x, localPoint->y, localPoint->z, 1, 1);
		i++;
	}
	int faceIndex = 0;
	while (faceIndex < this->numStraps)
	{
		int firstIndex = (2 * faceIndex);
		int secondIndex = (firstIndex + 3);
		if (secondIndex >= (2 * this->numStraps))
		{
			secondIndex = (secondIndex - (2 * this->numStraps));
		}
		this->createTriFace(this->boxVertex, this->topVertices[firstIndex], this->topVertices[secondIndex]);
		this->createTriFace(this->boxVertex, this->topVertices[secondIndex], this->topVertices[firstIndex]);
		faceIndex++;
	}
}

std::shared_ptr<Vertex> Cords1::createVertex(double x, double y, double z, double u, double v)
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

std::shared_ptr<Face> Cords1::createTriFace(std::shared_ptr<Vertex> a, std::shared_ptr<Vertex> b, std::shared_ptr<Vertex> c)
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

void Cords1::destroy()
{
	//var v:Vertex;
	//var v1:Vector3;
	//var buf:Vertex;
	//var buf1:Face;
	//this->boxLocalPoint = nullptr;
	/*
	if (this->topVertices != null)
	{
		for each (v in this->topVertices)
		{
			if (v != null)
			{
				v = null;
			};
		};
		this->topVertices = null;
	}
	*/

	/*
	if (this->topLocalPoints != null)
	{
		for each (v1 in this->topLocalPoints)
		{
			if (v1 != null)
			{
				v1 = null;
			}
		}
		this->topLocalPoints = null;
	}
	*/

	//this->boxVertex = null;
	/*
	if (vertexList != null)
	{
		while (vertexList.next != null)
		{
			buf = vertexList;
			vertexList = null;
			vertexList = buf.next;
		};
		vertexList = null;
	}

	if (faceList != null)
	{
		while (faceList.next != null)
		{
			buf1 = faceList;
			if (faceList.wrapper != null)
			{
				faceList.wrapper.vertex = null;
			};
			faceList.wrapper = null;
			faceList = null;
			faceList = buf1.next;
		};
		faceList = null;
	};
	captureListeners = null;
	bubbleListeners = null;
	*/

	Mesh::destroy();
}


std::shared_ptr<Cords1> new_Cords1(double radius, double boxHalfSize, int numStraps, Object3D *box, Object3D *parachute)
{
	return std::make_shared<Cords1>(Object3D::TT(Cords1::TYPE), radius, boxHalfSize, numStraps, box, parachute);
}
