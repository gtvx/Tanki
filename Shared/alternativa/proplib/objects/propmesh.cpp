#include "propmesh.h"
#include "alternativa/proplib/utils/texturebytedatamap.h"
#include "alternativa/utils/texturebytedata.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/objects/occluder.h"
#include "alternativa/engine3d/core/face.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/utils/bytearraymap.h"
#include "alternativa/engine3d/loaders/parser3ds.h"
#include <QHashIterator>



const QString PropMesh::DEFAULT_TEXTURE = "$$$_DEFAULT_TEXTURE_$$$";
double PropMesh::threshold = 0.01;
double PropMesh::occluderDistanceThreshold = 0.01;
double PropMesh::occluderAngleThreshold = 0.01;
double PropMesh::occluderConvexThreshold = 0.01;
int PropMesh::occluderUvThreshold = 1;
double PropMesh::meshDistanceThreshold = 0.001;
double PropMesh::meshUvThreshold = 0.001;
double PropMesh::meshAngleThreshold = 0.001;
double PropMesh::meshConvexThreshold = 0.01;


PropMesh::PropMesh(QByteArray &modelData,
				   const QString &objectName,
				   QHash<QString, QString> &textureFiles,
				   ByteArrayMap *files,
				   TextureByteDataMap *imageMap) :
	PropObject(PropObjectType::MESH)
{
	this->occluders = nullptr;
	this->parseModel(modelData, objectName, textureFiles, files, imageMap);
}

void PropMesh::parseModel(QByteArray &modelData,
						  const QString &objectName,
						  QHash<QString, QString> &textureFiles,
						  ByteArrayMap *files,
						  TextureByteDataMap *imageMap)
{
	std::shared_ptr<Mesh> mesh = this->processObjects(modelData, objectName);

	this->initMesh(mesh);

	std::shared_ptr<Object3D> object = std::dynamic_pointer_cast<Object3D>(mesh);

	this->setObject3D(object);

	QString defaultTextureFileName = this->getTextureFileName(mesh);

	if (((defaultTextureFileName.isEmpty()) && (textureFiles.isEmpty())))
	{
		//throw (new Error("PropMesh: no textures found"));
		throw 341435;
	}

	if (!defaultTextureFileName.isEmpty())
	{
		textureFiles[PropMesh::DEFAULT_TEXTURE] = defaultTextureFileName;
	}

	this->textures = std::make_shared<TextureByteDataMap>();

	{
		QHashIterator<QString, QString> i(textureFiles);
		while (i.hasNext())
		{
			i.next();
			const QString &textureName = i.key();
			const QString &textureFileName = i.value();

			 std::shared_ptr<TextureByteData> textureByteData;

			if (imageMap == nullptr)
			{
				textureByteData = std::make_shared<TextureByteData>(files->getValue(textureFileName.toLower()), QByteArray());
			}
			else
			{
				textureByteData = imageMap->getValue(textureFileName);
			}

			this->textures->putValue(textureName, textureByteData);
		}
	}
}

std::shared_ptr<Mesh> PropMesh::processObjects(QByteArray &modelData, const QString &objectName)
{
	Parser3DS parser;
	parser.parse(modelData, "");

	QVector<std::shared_ptr<Object3D>> objects = parser.objects;

	int numObjects = objects.length();

	std::shared_ptr<Mesh> mesh;

	for (int i = 0; i < numObjects; i++)
	{
		std::shared_ptr<Object3D> currObject = objects.at(i);
		QString currObjectName = currObject->getName().toLower();
		if (currObjectName.indexOf("occl") == 0)
		{
			std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(currObject);
			this->addOccluder(mesh);
		}
		else
		{
			if (objectName == currObjectName)
			{
				mesh = std::dynamic_pointer_cast<Mesh>(currObject);
			}
		}
	}

	return mesh != nullptr ? mesh : std::dynamic_pointer_cast<Mesh>(objects.at(0));
}

QString PropMesh::getTextureFileName(std::shared_ptr<Mesh> &mesh)
{
	std::shared_ptr<Face> face = mesh->faceList();

	while (face != nullptr)
	{
		Material *material = face->material.get();

		if (material->isType(TextureMaterial::TYPE))
		{
			TextureMaterial *textureMaterial = (TextureMaterial*)material;
			return textureMaterial->diffuseMapURL;
		}

		face = face->next;
	}

	return nullptr;
}

void PropMesh::addOccluder(std::shared_ptr<Mesh> &mesh)
{
	mesh->weldVertices(occluderDistanceThreshold, occluderUvThreshold);
	mesh->weldFaces(occluderAngleThreshold, occluderUvThreshold, occluderConvexThreshold);

	std::shared_ptr<Occluder> occluder = new_Occluder();
	occluder->createForm(mesh);

	occluder->position.x = mesh->position.x;
	occluder->position.y = mesh->position.y;
	occluder->position.z = mesh->position.z;
	occluder->rotation.x = mesh->rotation.x;
	occluder->rotation.y = mesh->rotation.y;
	occluder->rotation.z = mesh->rotation.z;

	if (this->occluders == nullptr)
	{
		this->occluders = new QVector<std::shared_ptr<Occluder>>;
	}

	this->occluders->append(occluder);
}

void PropMesh::initMesh(std::shared_ptr<Mesh> mesh)
{
	mesh->weldVertices(meshDistanceThreshold, meshUvThreshold);
	mesh->weldFaces(meshAngleThreshold, meshUvThreshold, meshConvexThreshold);
	mesh->threshold = threshold;
}

void PropMesh::traceProp()
{	
	PropObject::traceProp();

	//for (textureName in this->textures)
	//{
		//TextureByteData *textureData = this->textures[textureName];
	//}

}
