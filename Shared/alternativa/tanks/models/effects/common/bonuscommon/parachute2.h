#ifndef PARACHUTE2_H
#define PARACHUTE2_H

#include "alternativa/engine3d/core/object3dcontainer.h"

class Mesh;

class Parachute2 : public Object3DContainer
{
	double ownAlpha;
	double alphaMultiplier;

	std::shared_ptr<Mesh> mesh1;
	std::shared_ptr<Mesh> mesh2;

public:	
	static const double RADIUS;
	static const int NUM_STRAPS;

	static const T *TYPE;
	const T *__type_next;

	Parachute2(const TT &t, Mesh *outerMeshSource, Mesh *innerMeshSource);

private:
	void addMesh(Mesh *mesh);

public:
	void recycle();
	double getAlpha() { return this->ownAlpha; }
	void setAlpha(double value);

private:
	void updateAlpha();

public:
	void readPosition(Vector3 *result);
	void setAlphaMultiplier(double value);
};

std::shared_ptr<Parachute2> new_Parachute2(Mesh *outerMeshSource, Mesh *innerMeshSource);

#endif // PARACHUTE2_H
