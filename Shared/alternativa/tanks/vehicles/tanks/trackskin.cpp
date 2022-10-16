#include "trackskin.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/core/vertex.h"
#include "mymath.h"
#include "flash/geom/matrix.h"
#include "alternativa/engine3d/materials/material.h"
#include "alternativa/engine3d/materials/uvmatrixprovider.h"
#include "alternativa/tanks/materials/trackmaterial.h"
#include <QHash>
#include <QDebug>
#include "qhash.h"
#include "qhash_ptr.h"



static double getRatioForVertices(Vertex *vertex1, Vertex *vertex2)
{
	double dx = vertex1->x - vertex2->x;
	double dy = vertex1->y - vertex2->y;
	double dz = vertex1->z - vertex2->z;
	double vertexDistanceXYZ = MyMath::sqrt((((dx * dx) + (dy * dy)) + (dz * dz)));
	double du = vertex1->u - vertex2->u;
	double dv = vertex1->v - vertex2->v;
	double vertexDistanceUV = MyMath::sqrt(((du * du) + (dv * dv)));
	return vertexDistanceUV / vertexDistanceXYZ;
}


static double getRatio(Face *face)
{
	QVector<std::shared_ptr<Vertex>> *_vertices = face->getVertices();
	double v = getRatioForVertices(_vertices->at(0).get(), _vertices->at(1).get());
	delete _vertices;
	return v;
}




TrackSkin::TrackSkin() //4
{
	uvsProvider = nullptr;
	ratio = 0;
	distance = 0;
	vertices = nullptr;
	material = nullptr;
}

void TrackSkin::addFace(std::shared_ptr<Face> _face)
{
	this->faces.append(_face);
}

void TrackSkin::init()
{
	double co = 0;
	QHash<std::shared_ptr<Vertex>, bool> dictionary;

	for (std::shared_ptr<Face> polygon : this->faces)
	{
		QVector<std::shared_ptr<Vertex>> *vertices = polygon->getVertices();

		for (std::shared_ptr<Vertex> vertex : *vertices)
		{
			dictionary[vertex] = true;
		}

		delete vertices;

		co = co + getRatio(polygon.get());
	}

	this->ratio = co / this->faces.length();

	this->vertices = new QVector<std::shared_ptr<Vertex>>;

	QHashIterator<std::shared_ptr<Vertex>, bool> i(dictionary);
	while (i.hasNext())
	{
		i.next();
		this->vertices->append(i.key());
	}
}

void TrackSkin::move(double delta)
{
	this->distance = this->distance + (delta * this->ratio);
	if (this->uvsProvider != nullptr)
	{
		Matrix *m = this->uvsProvider->getMatrix();
		m->tx = this->distance;
	}
}

void TrackSkin::setMaterial(std::shared_ptr<Material> newMaterial)
{
	newMaterial->setName("track");

	for (std::shared_ptr<Face> face : this->faces)
	{
		face->material = newMaterial;
	}

	if (newMaterial->isType(TrackMaterial::TYPE))
	{
		this->material = std::dynamic_pointer_cast<TrackMaterial>(newMaterial);
		std::shared_ptr<TrackMaterial> trackMat = this->material;
		this->uvsProvider = trackMat->uvMatrixProvider;
	}
}

void TrackSkin::destroy()
{
	/*
	var f:Face;
	var v:Vertex;
	if (this.faces != null)
	{
		for each (f in this.faces)
		{
			f.destroy();
			f = null;
		}
		this.faces = null;
	}
	if (this.vertices != null)
	{
		for each (v in this.vertices)
		{
			v = null;
		}
		this.vertices = null;
	}
	*/
}
