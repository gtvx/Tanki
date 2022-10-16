#ifndef CORDS1_H
#define CORDS1_H

class Object3D;
class Vertex;
class Face;


#include "alternativa/engine3d/objects/mesh.h"
#include <memory>
#include <QVector>


class Cords1 : public Mesh
{
	Object3D *box;
	Object3D *parachute;
	int numStraps;
	std::shared_ptr<Vertex> *topVertices;
	Vector3 *topLocalPoints;
	std::shared_ptr<Vertex> boxVertex;
	Vector3 boxLocalPoint;
public:

	static const T *TYPE;
	const T *__type_next;

	Cords1(const TT &t, double radius, double boxHalfSize, int numStraps, Object3D *box, Object3D *parachute);
	~Cords1();

	void updateVertices();

	void destroy();

private:
	void createGeometry(double radius, double boxHalfSize);
	std::shared_ptr<Vertex>	createVertex(double x, double y, double z, double u, double v);
	std::shared_ptr<Face> createTriFace(std::shared_ptr<Vertex> a, std::shared_ptr<Vertex> b, std::shared_ptr<Vertex> c);
};

std::shared_ptr<Cords1> new_Cords1(double radius, double boxHalfSize, int numStraps, Object3D *box, Object3D *parachute);

#endif // CORDS1_H
