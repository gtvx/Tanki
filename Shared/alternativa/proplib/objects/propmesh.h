#ifndef PROPMESH_H
#define PROPMESH_H

#include "propobject.h"
#include "PropObjectType.h"
#include <memory>
#include <QString>
#include <QVector>


class TextureByteDataMap;
class Occluder;
class ByteArrayMap;
class Mesh;


class PropMesh : public PropObject
{
public:

	static const QString DEFAULT_TEXTURE;
	static double threshold;
	static double occluderDistanceThreshold;
	static double occluderAngleThreshold;
	static double occluderConvexThreshold;
	static int occluderUvThreshold;
	static double meshDistanceThreshold;
	static double meshUvThreshold;
	static double meshAngleThreshold;
	static double meshConvexThreshold;

	std::shared_ptr<TextureByteDataMap> textures;
	QVector<std::shared_ptr<Occluder>> *occluders;

	PropMesh(QByteArray &modelData,
			 const QString &objectName,
			 QHash<QString, QString> &textureFiles,
			 ByteArrayMap *files,
			 TextureByteDataMap *imageMap);

	void parseModel(QByteArray &modelData,
					const QString &objectName,
					QHash<QString, QString> &textureFiles,
					ByteArrayMap *files,
					TextureByteDataMap *imageMap);

private:
	std::shared_ptr<Mesh> processObjects(QByteArray &modelData, const QString &objectName);
	QString getTextureFileName(std::shared_ptr<Mesh> &mesh);
	void addOccluder(std::shared_ptr<Mesh> &mesh);
	void initMesh(std::shared_ptr<Mesh> mesh);

public:
	void traceProp() override;
};

#endif // PROPMESH_H
