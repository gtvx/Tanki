#ifndef TRACKSKIN_H
#define TRACKSKIN_H

#include <QVector>
#include <memory>

class UVMatrixProvider;
class Face;
class Vertex;
class TrackMaterial;
class Material;


class TrackSkin
{

	UVMatrixProvider *uvsProvider;

	QVector<std::shared_ptr<Face>> faces;
	QVector<std::shared_ptr<Vertex>> *vertices;

	double ratio;
	double distance;

public:

	std::shared_ptr<TrackMaterial> material;
	TrackSkin();
	void addFace(std::shared_ptr<Face> _face);
	void init();
	void move(double delta);
	void setMaterial(std::shared_ptr<Material> newMaterial);
	void destroy();
};

#endif // TRACKSKIN_H
