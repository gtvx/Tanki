#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <qglobal.h>
#include <QVector>
#include <QHash>
#include <memory>
#include "qhash_ptr.h"


class CollisionPrimitive;
class PropLibRegistry;
class Light3D;
class Occluder;
class Object3D;
class Mesh;
class PropMesh;
class QDomElement;
class PropObject;
class MaterialUsersRegistry;
class PropSprite;
class Sprite3D;
class BatchTextureBuilder;



class MapLoader
{
public:
	MapLoader();
	std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> _collisionPrimitives;
	std::shared_ptr<PropLibRegistry> propLibRegistry;
	QVector<std::shared_ptr<Light3D>> _lights;
	QVector<std::shared_ptr<Occluder>> _occluders;

	QVector<std::shared_ptr<Object3D>> meshes;


	QVector<std::shared_ptr<Object3D>> _objects;
	QVector<std::shared_ptr<Sprite3D>> _sprites;

	
	QHash<std::shared_ptr<Mesh>, QString> textureNameByMesh;
	QHash<std::shared_ptr<Mesh>, std::shared_ptr<PropMesh>> propObjectByMesh;
	std::shared_ptr<Mesh> christmasTree;


	bool load(const QString &filename,std::shared_ptr<PropLibRegistry> libRegistr);

	std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> getCollisionPrimitives()
	{
		return _collisionPrimitives;
	}

	QVector<std::shared_ptr<Object3D>>* objects()
	{
		return &this->_objects;
	}

	QVector<std::shared_ptr<Occluder>>* occluders()
	{
		return &this->_occluders;
	}

	QVector<std::shared_ptr<Sprite3D>>* sprites()
	{
		return &this->_sprites;
	}

	QVector<std::shared_ptr<Light3D>>* lights()
	{
		return &this->_lights;
	}


private:

	std::shared_ptr<MaterialUsersRegistry> materialUsersRegistry;
	std::shared_ptr<BatchTextureBuilder> batchTextureBuilder;
	double mipMapResolution;

	void parseProps(const QDomElement &xml);
	void parsePropMesh(const QDomElement &propXml, std::shared_ptr<PropMesh> &propMesh);
	void parseProp(const QDomElement &propXml);
	void makeBSPs();
	void parsePropSprite(const QDomElement &propXml, std::shared_ptr<PropSprite> &propSprite);
	void runTextureBuilder();
	void onTexturesComplete();
	void complete();
};

#endif // MAPLOADER_H
