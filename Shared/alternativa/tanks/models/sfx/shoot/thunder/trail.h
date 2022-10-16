#ifndef TRAIL_H
#define TRAIL_H

#include "alternativa/engine3d/objects/mesh.h"


class Trail : public Mesh
{
public:
	static const T *TYPE;
	const T *__type_next;


	Trail(const TT &t, double scale, std::shared_ptr<Material> material);

private:
	std::shared_ptr<Vertex> createVertex(double x, double y, double z, double u, double v);
	std::shared_ptr<Face> createFace(std::shared_ptr<Vertex> &a, std::shared_ptr<Vertex> &b, std::shared_ptr<Vertex> &c);
};

std::shared_ptr<Trail> new_Trail(double scale, std::shared_ptr<Material> material);


#endif // TRAIL_H
