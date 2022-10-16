#ifndef CORDS2_H
#define CORDS2_H

#include "alternativa/engine3d/objects/mesh.h"

class Material;
class BonusMesh;
class Parachute2;


class Cords2 : public Mesh
{

	BonusMesh *bonusMesh;
	int numStraps;
	std::shared_ptr<Vertex> *topVertices;
	Vector3 *topLocalPoints;
	std::shared_ptr<Vertex> boxVertex;
	Vector3 boxLocalPoint;
	Parachute2 *parachute;
	double ownAlpha = 1;
	double alphaMultiplier = 1;
public:

	static const T *TYPE;
	const T *__type_next;

	Cords2(const TT &t, double radius, double boxHalfSize, int numStraps, Material *material);
	~Cords2();

	void init(BonusMesh *bonusMesh, Parachute2 *parachute);
	void updateVertices();
	void createGeometry(double radius, double boxHalfSize);

private:
	std::shared_ptr<Vertex> createVertex(double x, double y, double z, double u, double v);
	std::shared_ptr<Face> createTriFace(std::shared_ptr<Vertex> a, std::shared_ptr<Vertex> b, std::shared_ptr<Vertex> c);

public:
	void recycle();
	void setAlpha(double value);
	void updateAlpha();
	void readPosition(Vector3 *result);
	void setAlphaMultiplier(double value);
};

std::shared_ptr<Cords2> new_Cords2(double radius, double boxHalfSize, int numStraps, Material *material);

#endif // CORDS2_H
