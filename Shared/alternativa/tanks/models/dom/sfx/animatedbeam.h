#ifndef ANIMATEDBEAM_H
#define ANIMATEDBEAM_H

#include "alternativa/engine3d/objects/mesh.h"

class TextureMaterial;


class AnimatedBeam : public Mesh
{
	Vertex *a;
	Vertex *b;
	Vertex *c;
	Vertex *d;
	Vertex *e;
	Vertex *f;
	Vertex *g;
	Vertex *h;
	Vertex *i;
	Vertex *j;
	Vertex *k;
	Vertex *l;

	double unitLength;
	double vOffset;

public:

	static const T *TYPE;
	const T *__type_next;


	double animationSpeed;

	AnimatedBeam(const TT &t, double param1, double param2, double unitLength, double animationSpeed);

	void setMaterials(TextureMaterial *t1, TextureMaterial *t2);

private:
	void writeVertices();
	void updateV();
	void clear();

public:
	void setTipLength(double param1);
	void resize(double width, double length);
	void setWidth(double width);
	void setLength(double length);
	void setURange(double param1);
	void update(double v);
	void setUnitLength(double value);
};

std::shared_ptr<AnimatedBeam> new_AnimatedBeam(double param1, double param2, double unitLength, double animationSpeed);

#endif // ANIMATEDBEAM_H
