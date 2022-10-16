#ifndef SIMPLEPLANE_H
#define SIMPLEPLANE_H

#include "alternativa/engine3d/objects/mesh.h"

class SimplePlane : public Mesh
{
	double originX;
	double originY;

protected:

	Vertex *a;
	Vertex *b;
	Vertex *c;
	Vertex *d;

public:

	static const T *TYPE;
	const T *__type_next;



	SimplePlane(const TT &t, double param1, double param2, double param3, double param4);
	void writeVertices();
	void setUVs(double param1, double param2, double param3, double param4, double param5, double param6, double param7, double param8);
	void setWidth(double value);
	double getLength();
	void setLength(double value);
	void resize(double width, double length);
};


std::shared_ptr<SimplePlane> new_SimplePlane(double param1, double param2, double param3, double param4);


#endif // SIMPLEPLANE_H
