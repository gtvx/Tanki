#include "tailtrail.h"

const Object3D::T *TailTrail::TYPE = new Object3D::T("TailTrail");

TailTrail::TailTrail(const TT &t, double width, double length) :
	SimplePlane(TT(t.type, this->TYPE), width, length, 0.5, 1),
	__type_next(t.type_next)
{
	this->setUVs(1, 1, 1, 0, 0, 0, 0, 1);
}


std::shared_ptr<TailTrail> new_TailTrail(double width, double length)
{
	return std::make_shared<TailTrail>(Object3D::TT(TailTrail::TYPE), width, length);
}

