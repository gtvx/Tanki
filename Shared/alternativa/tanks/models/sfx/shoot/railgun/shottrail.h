#ifndef SHOTTRAIL_H
#define SHOTTRAIL_H

#include "alternativa/engine3d/objects/mesh.h"


class ShotTrail : public Mesh
{
	std::shared_ptr<Vertex> a;
	std::shared_ptr<Vertex> b;
	std::shared_ptr<Vertex> c;
	std::shared_ptr<Vertex> d;
	std::shared_ptr<Face> face;
	double bottomV;
	double distanceV;
public:

	static const T *TYPE;
	const T *__type_next;


	ShotTrail(const TT &t);

	void init(double param1, double param2, std::shared_ptr<Material> &material, double param4);
	void clear();
	void update(double param1);
};

std::shared_ptr<ShotTrail> new_ShotTrail();

#endif // SHOTTRAIL_H
