#include "simpleplane.h"
#include "alternativa/engine3d/core/vertex.h"


const Object3D::T *SimplePlane::TYPE = new Object3D::T("SimplePlane");


SimplePlane::SimplePlane(const TT &t, double param1, double param2, double param3, double param4) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{

	this->originX = param3;
	this->originY = param4;
	this->bound.MinX = -(param3) * param1;
	this->bound.MaxX = this->bound.MinX + param1;
	this->bound.MinY = -param4 * param2;
	this->bound.MaxY = this->bound.MinY + param2;
	this->bound.MinZ = 0;
	this->bound.MaxZ = 0;

	double _loc5_[12] =
	{
		bound.MinX,
		bound.MinY,
		0,
		bound.MaxX,
		bound.MinY,
		0,
		bound.MaxX,
		bound.MaxY,
		0,
		bound.MinX,
		bound.MaxY,
		0
	};

	const double _loc6_[8] = {0, 1, 1, 1, 1, 0, 0, 0};
	const int _loc7_[5] = {4, 0, 1, 2, 3};

	this->addVerticesAndFaces(_loc5_, 12, _loc6_, 8, _loc7_, 5, true);
	this->calculateFacesNormals();
	this->writeVertices();

}

void SimplePlane::writeVertices()
{
	Vertex *v[4];
	this->getVertices(v, 4);
	this->a = v[0];
	this->b = v[1];
	this->c = v[2];
	this->d = v[3];
}

void SimplePlane::setUVs(double param1, double param2, double param3, double param4, double param5, double param6, double param7, double param8)
{
	this->a->u = param1;
	this->a->v = param2;
	this->b->u = param3;
	this->b->v = param4;
	this->c->u = param5;
	this->c->v = param6;
	this->d->u = param7;
	this->d->v = param8;
}

void SimplePlane::setWidth(double value)
{
	this->bound.MinX = (this->a->x = (this->d->x = (-(this->originX) * value)));
	this->bound.MaxX = (this->b->x = (this->c->x = (bound.MinX + value)));
}

double SimplePlane::getLength()
{
	return bound.MaxY - bound.MinY;
}

void SimplePlane::setLength(double value)
{
	bound.MinY = this->a->y = (this->b->y = (-(this->originY) * value));
	bound.MaxY = this->d->y = (this->c->y = (bound.MinY + value));
}

void SimplePlane::resize(double width, double length)
{
	this->setWidth(width);
	this->setLength(length);
}

std::shared_ptr<SimplePlane> new_SimplePlane(double param1, double param2, double param3, double param4)
{
	return std::make_shared<SimplePlane>(Object3D::TT(SimplePlane::TYPE), param1, param2, param3, param4);
}
