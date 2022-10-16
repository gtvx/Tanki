#ifndef TRAIL1_H
#define TRAIL1_H

#include "alternativa/engine3d/objects/mesh.h"

class Vertex;

class Trail1 : public Mesh
{
	std::shared_ptr<Vertex> a;
	std::shared_ptr<Vertex> b;
	std::shared_ptr<Vertex> c;
	std::shared_ptr<Vertex> d;
	std::shared_ptr<Face> face;

public:

	static const T *TYPE;
	const T *__type_next;

	Trail1(const TT &t);

	void init(double param1, double param2, double param3, std::shared_ptr<Material> &material);
	void setWidth(double value);
	double getLength();
	void setLength(double value);

private:
	std::shared_ptr<Vertex> createVertex(double x,
										 double y,
										 double z,
										 double u,
										 double v);

	std::shared_ptr<Face> createQuad(std::shared_ptr<Vertex> v1,
									 std::shared_ptr<Vertex> v2,
									 std::shared_ptr<Vertex> v3,
									 std::shared_ptr<Vertex> v4);
};


std::shared_ptr<Trail1> new_Trail1();

#endif // TRAIL1_H
