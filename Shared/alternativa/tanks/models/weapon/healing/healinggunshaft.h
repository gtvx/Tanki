#ifndef HEALINGGUNSHAFT_H
#define HEALINGGUNSHAFT_H

#include "alternativa/engine3d/objects/mesh.h"


class HealingGunShaft : public Mesh
{
	std::shared_ptr<Vertex> a;
	std::shared_ptr<Vertex> b;
	std::shared_ptr<Vertex> c;
	std::shared_ptr<Vertex> d;
	std::shared_ptr<Face> face;
public:

	static const T *TYPE;
	const T *__type_next;

	HealingGunShaft(const TT &t);

	void setMaterial(std::shared_ptr<Material> &value);
	void init(double width, double length);
	double getLength();
	void setLength(double value);

private:
	std::shared_ptr<Vertex> createVertex(double x, double y, double z, double u, double v);

	std::shared_ptr<Face> createQuad(std::shared_ptr<Vertex> &a,
									 std::shared_ptr<Vertex> &b,
									 std::shared_ptr<Vertex> &c,
									 std::shared_ptr<Vertex> &d);

	void destroy() override;
};

std::shared_ptr<HealingGunShaft> new_HealingGunShaft();

#endif // HEALINGGUNSHAFT_H
