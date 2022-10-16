#include "maploader.h"
#include "_global.h"
#include "mymath.h"
#include "alternativa/math/vector3.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/physics/collision/primitives/collisionrect.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "alternativa/physics/collision/primitives/collisiontriangle.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/proplib/objects/propobject.h"
#include "alternativa/proplib/types/propgroup.h"
#include "alternativa/proplib/proplibrary.h"
#include "alternativa/proplib/objects/propmesh.h"
#include "alternativa/proplib/types/propdata.h"
#include "alternativa/proplib/types/propstate.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/proplib/proplibregistry.h"
#include "alternativa/engine3d/lights/omnilight.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "alternativa/engine3d/objects/occluder.h"
#include "alternativa/engine3d/objects/bsp.h"
#include "alternativa/utils/texturebytedata.h"
#include "alternativa/proplib/utils/texturebytedatamap.h"
#include "alternativa/proplib/objects/propsprite.h"
#include "alternativa/engine3d/objects/bsp.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/utils/ITextureConstructorListener.h"
#include "alternativa/utils/textureutils/textureconstructor.h"
#include "file.h"
#include <QDomDocument>
#include <QDomElement>
#include <QVector>
#include <QDebug>


class TextureMaterial;

class IMaterialUser
{
public:
	virtual void setMaterial(TextureMaterial*) = 0;
};




class Sprite3DWrapper : public IMaterialUser
{
	Sprite3D *sprite;
public:

	Sprite3DWrapper(Sprite3D *sprite)
	{
		this->sprite = sprite;
	}

	void setMaterial(TextureMaterial *material)
	{
		(void)material;
#ifdef GRAPHICS
		BitmapData *texture = material->getTexturePtr();
		this->sprite->material = material->ptr.lock();
		double scale = this->sprite->width;
		this->sprite->width = scale * texture->width();
		this->sprite->height = scale * texture->height();
		material->resolution = this->sprite->calculateResolution(texture->width(), texture->height());
#endif
	}
};

class BSPWrapper : public IMaterialUser
{
	std::shared_ptr<BSP> bsp;
public:
	BSPWrapper(std::shared_ptr<BSP> &bsp)
	{
		this->bsp = bsp;
	}

	void setMaterial(TextureMaterial *material)
	{
		this->bsp->setMaterialToAllFaces(material);
	}
};



class MaterialUserEntry
{
public:
	QVector<std::shared_ptr<IMaterialUser>> materialUsers;


	virtual std::shared_ptr<TextureByteData> getTextureData()
	{
		throw 543254;
		//throw (new Error("Not implemented"));
	}
};



class BSPMaterialUserEntry : public MaterialUserEntry
{
public:
	std::shared_ptr<PropMesh> propMesh;
	QString textureName;

	BSPMaterialUserEntry(std::shared_ptr<PropMesh> &propMesh, const QString &textureName)
	{
		this->propMesh = propMesh;
		this->textureName = textureName;
	}

	std::shared_ptr<TextureByteData> getTextureData() override
	{
#ifdef GRAPHICS
		std::shared_ptr<TextureByteData> ro = this->propMesh->textures->getValue(this->textureName);
		if (ro == nullptr)
		{
			//trace(this->textureName);
		}
		return ro;
#endif
		return nullptr;
	}
};

class Sprite3DMaterialUserEntry : public MaterialUserEntry
{
public:
	std::shared_ptr<PropSprite> propSprite;

	Sprite3DMaterialUserEntry(std::shared_ptr<PropSprite> &propSprite)
	{
		this->propSprite = propSprite;
	}

	std::shared_ptr<TextureByteData> getTextureData() override
	{
		return this->propSprite->textureData;
	}
};

class MaterialUsersRegistry
{
public:
	QVector<std::shared_ptr<BSPMaterialUserEntry>> bspEntries;
	QVector<std::shared_ptr<Sprite3DMaterialUserEntry>> spriteEntries;

	void addBSP(std::shared_ptr<PropMesh> &propMesh, const QString &textureName, std::shared_ptr<BSPWrapper> materialUser)
	{
		std::shared_ptr<BSPMaterialUserEntry> entry = nullptr;

		for (std::shared_ptr<BSPMaterialUserEntry> &currentEntry : this->bspEntries)
		{
			if (currentEntry->propMesh == propMesh && currentEntry->textureName == textureName)
			{
				entry = currentEntry;
				break;
			}
		}

		if (entry == nullptr)
		{
			entry = std::make_shared<BSPMaterialUserEntry>(propMesh, textureName);
			this->bspEntries.append(entry);
		}

		entry->materialUsers.append(materialUser);
	}

	void addSprite3D(std::shared_ptr<PropSprite> &propSprite, std::shared_ptr<Sprite3DWrapper> &wrapper)
	{
		std::shared_ptr<Sprite3DMaterialUserEntry> entry = nullptr;

		for (std::shared_ptr<Sprite3DMaterialUserEntry> &currentEntry : this->spriteEntries)
		{
			if (currentEntry->propSprite == propSprite)
			{
				entry = currentEntry;
				break;
			}
		}

		if (entry == nullptr)
		{
			entry = std::make_shared<Sprite3DMaterialUserEntry>(propSprite);
			this->spriteEntries.append(entry);
		}

		entry->materialUsers.append(wrapper);
	}
};




class IndexedTextureConstructor : public TextureConstructor
{
public:
	int index;
};



class BatchTextureBuilder : public ITextureConstructorListener
{
	QVector<MaterialUserEntry*> entries;
	QVector<IndexedTextureConstructor*> constructors;
	double mipMapResolution;
	int maxBatchSize;
	int batchSize;
	int firstBatchIndex;
	int batchCouner;
	int totalCounter;
	int count;
	bool useMipMap;

public:

	BatchTextureBuilder()
	{
		mipMapResolution = 0;
		maxBatchSize = 0;
		batchSize = 0;
		firstBatchIndex = 0;
		batchCouner = 0;
		totalCounter = 0;
	}

	virtual ~BatchTextureBuilder()
	{

	}


	void run(double mipMapResolution, int maxBatchSize, QVector<std::shared_ptr<BSPMaterialUserEntry>> &bspEntries, QVector<std::shared_ptr<Sprite3DMaterialUserEntry>> &spriteEntries)
	{
		(void)mipMapResolution;
		(void)maxBatchSize;
		(void)bspEntries;
		(void)spriteEntries;

		this->useMipMap = true; //(Main.osgi.getService(IBattleSettings) as IBattleSettings).enableMipMapping;

#ifdef GRAPHICS
		{
			for (std::shared_ptr<BSPMaterialUserEntry> &bspEntry : bspEntries)
			{
				std::shared_ptr<TextureByteData> textureData = bspEntry->getTextureData();

				TextureConstructor textureConstructor;
				textureConstructor.createTexture(textureData, nullptr);

				std::shared_ptr<TextureMaterial> textureMaterial = new_TextureMaterial();
				textureMaterial->init(false, true, ((this->useMipMap) ? MipMapping::PER_PIXEL : MipMapping::NONE), 1);
				textureMaterial->setTexture(textureConstructor.texture());
				for (std::shared_ptr<IMaterialUser> &materialUser : bspEntry->materialUsers)
				{
					materialUser->setMaterial(textureMaterial.get());
				}
			}

			for (std::shared_ptr<Sprite3DMaterialUserEntry> &spriteEntries : spriteEntries)
			{
				std::shared_ptr<TextureByteData> textureData = spriteEntries->getTextureData();

				TextureConstructor textureConstructor;
				textureConstructor.createTexture(textureData, nullptr);

				std::shared_ptr<TextureMaterial> textureMaterial = new_TextureMaterial();
				textureMaterial->init(false, true, ((this->useMipMap) ? MipMapping::PER_PIXEL : MipMapping::NONE), 1);
				textureMaterial->setTexture(textureConstructor.texture());

				for (std::shared_ptr<IMaterialUser> &materialUser : spriteEntries->materialUsers)
				{
					materialUser->setMaterial(textureMaterial.get());
				}
			}
		}
#endif
		return;


		/*
		this->mipMapResolution = mipMapResolution;
		this->maxBatchSize = maxBatchSize;

		//this->constructors = new QVector<IndexedTextureConstructor*>;
		this->constructors.reserve(maxBatchSize);

		for (int i = 0; i < maxBatchSize; i++)
			this->constructors.append(new IndexedTextureConstructor());

		for (BSPMaterialUserEntry *bspEntry : bspEntries)
			this->entries.append(bspEntry);

		for (Sprite3DMaterialUserEntry *spriteEntry : spriteEntries)
			this->entries.append(spriteEntry);

		this->count = 0;
		this->totalCounter = 0;
		this->firstBatchIndex = 0;
		this->createBatch();
		*/
	}

	void onTextureReady(TextureConstructor *constructor)
	{
		IndexedTextureConstructor *textureConstructor = (IndexedTextureConstructor*)constructor;
		std::shared_ptr<TextureMaterial> textureMaterial = new_TextureMaterial();
		textureMaterial->init(false, true, this->useMipMap ? MipMapping::PER_PIXEL : MipMapping::NONE, 1);
		textureMaterial->setTexture(textureConstructor->texture());
		for (std::shared_ptr<IMaterialUser> &materialUser : this->entries[textureConstructor->index]->materialUsers)
		{
			materialUser->setMaterial(textureMaterial.get());
		}
		count++;
	}

private:

	void createBatch()
	{
		this->batchCouner = 0;

		int nextIndex = this->firstBatchIndex + this->maxBatchSize;

		if (nextIndex > this->entries.length())
		{
			nextIndex = this->entries.length();
		}

		this->batchSize = (nextIndex - this->firstBatchIndex);
		for (int i = 0; i < this->batchSize; i++)
		{
			IndexedTextureConstructor *textureConstructor = this->constructors[i];
			textureConstructor->index = this->firstBatchIndex + i;
			std::shared_ptr<TextureByteData> textureData = this->entries.at(textureConstructor->index)->getTextureData();
			textureConstructor->createTexture(textureData, this);
		}
		this->firstBatchIndex = nextIndex;


		for (int i = 0; i < count; i++)
		{
			this->totalCounter++;
			this->batchCouner++;
			if (this->totalCounter == this->entries.length())
			{
				this->complete();
				return;
			}
			else
			{
				if (this->batchCouner == this->batchSize)
				{
					count = 0;
					this->createBatch();
					return;
				}
			}
		}
	}

	void complete()
	{
		this->constructors.clear();
		this->entries.clear();
		//dispatchEvent(new Event(Event.COMPLETE));
	}


};


#define STATIC_COLLISION_GROUP 0xFF



static bool isNaNVector(Vector3 *v)
{
	return MyMath::isNaN(v->x) || MyMath::isNaN(v->y) || MyMath::isNaN(v->z);
}

static double getDouble(const QDomNode &a, const char *name)
{
	bool ok;
	double value = a.firstChildElement(name).text().toDouble(&ok);
	if (ok == false) {
		throw 21;
	}
	return value;
}

static void readVector3(const QDomElement &xml, Vector3 *result)
{
	if (xml.isNull() == false)
	{
		result->x = getDouble(xml, "x");
		result->y = getDouble(xml, "y");
		result->z = getDouble(xml, "z");
	}
	else
	{
		result->reset(0, 0, 0);
	}
}


static std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> parseCollisionPrimitives(const QDomElement &xml)
{
	std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> collisionPrimitives = std::make_shared<QVector<std::shared_ptr<CollisionPrimitive>>>();

	Vector3 v0;
	Vector3 v1;
	Vector3 v2;
	Vector3 halfSize;
	Vector3 position;
	Vector3 rotation;
	Matrix3 rotationMatrix;

	QDomElement collision_geometry = xml.firstChildElement("collision-geometry");

	{
		QDomNodeList list = collision_geometry.elementsByTagName("collision-plane");
		for (int i = 0; i < list.count(); i++)
		{
			QDomNode item = list.at(i);
			Vector3 halfSize;
			halfSize.x = 0.5 * getDouble(item, "width");
			halfSize.y = 0.5 * getDouble(item, "length");
			halfSize.z = 0.;
			std::shared_ptr<CollisionPrimitive> primitive = std::make_shared<CollisionRect>(&halfSize, STATIC_COLLISION_GROUP);
			readVector3(item.firstChildElement("position"), &position);
			readVector3(item.firstChildElement("rotation"), &rotation);
			rotationMatrix.setRotationMatrix(rotation.x, rotation.y, rotation.z);
			primitive->transform.setFromMatrix3(&rotationMatrix, &position);
			collisionPrimitives->append(primitive);
		}
	}


	{
		QDomNodeList list = collision_geometry.elementsByTagName("collision-box");
		for (int i = 0; i < list.count(); i++)
		{
			QDomNode item = list.at(i);
			readVector3(item.firstChildElement("size"), &halfSize);
			halfSize.scale(0.5);
			readVector3(item.firstChildElement("position"), &position);
			readVector3(item.firstChildElement("rotation"), &rotation);
			rotationMatrix.setRotationMatrix(rotation.x, rotation.y, rotation.z);
			std::shared_ptr<CollisionPrimitive> primitive = std::make_shared<CollisionBox>(&halfSize, STATIC_COLLISION_GROUP);
			primitive->transform.setFromMatrix3(&rotationMatrix, &position);
			collisionPrimitives->append(primitive);
		}
	}

	{
		QDomNodeList list = collision_geometry.elementsByTagName("collision-triangle");
		for (int i = 0; i < list.count(); i++)
		{
			QDomNode item = list.at(i);
			readVector3(item.firstChildElement("v0"), &v0);
			readVector3(item.firstChildElement("v1"), &v1);
			readVector3(item.firstChildElement("v2"), &v2);
			if (!(isNaNVector(&v0) || isNaNVector(&v1) || isNaNVector(&v2)))
			{
				readVector3(item.firstChildElement("position"), &position);
				readVector3(item.firstChildElement("rotation"), &rotation);
				rotationMatrix.setRotationMatrix(rotation.x, rotation.y, rotation.z);
				std::shared_ptr<CollisionPrimitive> primitive = std::make_shared<CollisionTriangle>(&v0, &v1, &v2, STATIC_COLLISION_GROUP);
				primitive->transform.setFromMatrix3(&rotationMatrix, &position);
				collisionPrimitives->append(primitive);
			}
		}
	}

	return collisionPrimitives;
}



static std::shared_ptr<PropObject> getPropObject(PropLibRegistry *propLibRegistry, const QDomElement &propXml)
{
	(void)propLibRegistry;
	(void)propXml;

#ifdef GRAPHICS
	QString libName = propXml.attribute("library-name");
	QString groupName = propXml.attribute("group-name");
	QString propName = propXml.attribute("name");

	PropLibrary *library = propLibRegistry->getLibrary(libName);

	if (library == nullptr)
	{

		//throw (new Error(("Library not found " + libName)));
		throw 563443;
	}

	PropGroup *group = library->getRootGroup()->getGroupByName(groupName);

	if (group == nullptr)
	{
		//throw (new Error(("Group not found " + groupName)));
		throw 3456345;
	}

	PropData *propData = group->getPropByName(propName);

	if (propData == nullptr)
	{
		//throw (new Error(("Prop data not found " + propName)));
		throw 513432;
	}

	return propData->getDefaultState()->getDefaultObject();
#endif
	return nullptr;
}

#ifdef GRAPHICS
static QString xmlToString(const QDomElement &xml, const QString &defaultValue)
{
	if (xml.isNull())
	{
		return defaultValue;
	}

	QString str = xml.text();
	return str.isEmpty() ? defaultValue : str;
}
#endif



void MapLoader::parsePropMesh(const QDomElement &propXml, std::shared_ptr<PropMesh> &propMesh)
{
	(void)propXml;
	(void)propMesh;

#ifdef GRAPHICS
	QString textureName = xmlToString(propXml.firstChildElement("texture-name"), PropMesh::DEFAULT_TEXTURE);

	if (textureName == "invisible")
	{
		return;
	}


	std::shared_ptr<Mesh> mesh = Mesh_clone(std::dynamic_pointer_cast<Mesh>(propMesh->getObject3D()));



	Vector3 position;
	Vector3 rotation;

	readVector3(propXml.firstChildElement("position"), &position);
	mesh->position.x = position.x;
	mesh->position.y = position.y;
	mesh->position.z = position.z;


	mesh->rotation.z = getDouble(propXml.firstChildElement("rotation"), "z");





	if (propXml.firstChildElement("texture-name").text() == "pumpkin")
	{
		std::shared_ptr<OmniLight> omni = new_OmniLight(0xFF8000, 500, 1000);
		omni->position.x = mesh->position.x;
		omni->position.y = mesh->position.y;
		omni->position.z = mesh->position.z + 100;
		this->_lights.append(omni);
	}

	if (propXml.attribute("name") == "Elka")
	{
		this->christmasTree = mesh;
	}

	this->textureNameByMesh[mesh] = textureName;
	this->propObjectByMesh[mesh] = propMesh;
	this->meshes.append(mesh);

	if (propMesh->occluders != nullptr)
	{
		Matrix4 objectMatrix;
		objectMatrix.setMatrix(&position, &rotation);

		for (std::shared_ptr<Occluder> &occluder : *propMesh->occluders)
		{
			Matrix4 matrix;
			matrix.copy(&occluder->matrix);

			matrix.append(&objectMatrix);

			std::shared_ptr<Occluder> mapOccluder = Occluder_clone(occluder);


			mapOccluder->matrix.copy(&matrix);

			this->_occluders.append(mapOccluder);
		}
	}
#endif
}

void MapLoader::parsePropSprite(const QDomElement &propXml, std::shared_ptr<PropSprite> &propSprite)
{
	std::shared_ptr<Sprite3D> sprite = std::dynamic_pointer_cast<Sprite3D>(Object3DAll_clone(propSprite->object));

	Vector3 position;
	readVector3(propXml.firstChildElement("position"), &position);

	sprite->position.x = position.x;
	sprite->position.y = position.y;
	sprite->position.z = position.z;
	sprite->width = propSprite->scale;
	sprite->softAttenuation = 80;
	/*
	if (propXml.attribute("name").indexOf("Shar") >= 0)
	{
		ToyLight *lighting = ToyLight.getRandomToy();
		std::shared_ptr<Sprite3D> sprite_ = new_Sprite3D(150, 150, new TextureMaterial(lighting.bd));
		sprite_->softAttenuation = 80;
		sprite_->position.x = sprite->position.x;
		sprite_->position.y = sprite->position.y;
		sprite_->position.z = sprite->position.z;
		this->_sprites.append(sprite_);
		std::shared_ptr<Light3D> omni = new_OmniLight(lighting.colorLighting, 200, 850);
		omni->position.x = sprite->position.x;
		omni->position.y = sprite->position.y;
		omni->position.z = sprite->position.z - 300;
		this->_lights.append(omni);
		//this->lightings.push(new ToyLightAnimation(omni));
		return;
	}
	*/
	this->_sprites.append(sprite);
	auto s = std::make_shared<Sprite3DWrapper>(sprite.get());
	this->materialUsersRegistry->addSprite3D(propSprite, s);
}


void MapLoader::parseProp(const QDomElement &propXml)
{
	std::shared_ptr<PropObject> propObject = getPropObject(this->propLibRegistry.get(), propXml);

	if (propObject->getType() == PropObjectType::MESH) {
		auto p = std::dynamic_pointer_cast<PropMesh>(propObject);
		this->parsePropMesh(propXml, p);
	} else if (propObject->getType() == PropObjectType::SPRITE) {
		auto p = std::dynamic_pointer_cast<PropSprite>(propObject);
		this->parsePropSprite(propXml, p);
	}
}


void MapLoader::parseProps(const QDomElement &xml)
{
	this->materialUsersRegistry = std::make_shared<MaterialUsersRegistry>();

	QDomElement static_geometry = xml.firstChildElement("static-geometry");

	QDomNodeList list = static_geometry.elementsByTagName("prop");
	for (int i = 0; i < list.count(); i++)
	{
		QDomNode propXML = list.at(i);
		this->parseProp(propXML.toElement());
	}
}

void MapLoader::makeBSPs()
{
#ifdef GRAPHICS
	for (std::shared_ptr<Object3D> &object : this->meshes)
	{
		std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(object);

		std::shared_ptr<BSP> bsp = new_BSP();
		bsp->setName("static");
		bsp->createTree(mesh, true);
		bsp->position.x = mesh->position.x;
		bsp->position.y = mesh->position.y;
		bsp->position.z = mesh->position.z;
		bsp->rotation.z = mesh->rotation.z;

		this->materialUsersRegistry->addBSP(this->propObjectByMesh[mesh],
											this->textureNameByMesh[mesh],
											std::make_shared<BSPWrapper>(bsp));

		this->_objects.append(bsp);
	}

	this->meshes.clear();
	//clearDictionary(this->textureNameByMesh);
	//clearDictionary(this->propObjectByMesh);
#endif
}


MapLoader::MapLoader()
{
	_collisionPrimitives = nullptr;
	christmasTree = nullptr;
	mipMapResolution = 5;
}

bool MapLoader::load(const QString &filename, std::shared_ptr<PropLibRegistry> libRegistry)
{
	this->propLibRegistry = libRegistry;

	QByteArray data;
	if (!File::read(filename, data))
		return false;

	QDomDocument doc;
	if (doc.setContent(data) == false)
		return false;

	QDomElement xml = doc.documentElement();

#ifdef GRAPHICS
	this->parseProps(xml);

	this->makeBSPs();
#endif

	_collisionPrimitives = parseCollisionPrimitives(xml);
	if (_collisionPrimitives == nullptr)
		return false;

#ifdef GRAPHICS
	runTextureBuilder();
#endif

	return true;
}





void MapLoader::runTextureBuilder()
{
	const int MAX_BATCH_SIZE = 20;

	this->batchTextureBuilder = std::make_shared<BatchTextureBuilder>();
	//this->batchTextureBuilder->addEventListener(Event.COMPLETE, this.onTexturesComplete);
	this->batchTextureBuilder->run(this->mipMapResolution,
								   MAX_BATCH_SIZE,
								   this->materialUsersRegistry->bspEntries,
								   this->materialUsersRegistry->spriteEntries);
	this->onTexturesComplete();
}

void MapLoader::onTexturesComplete()
{
	//this->batchTextureBuilder->removeEventListener(Event.COMPLETE, this.onTexturesComplete);
	this->batchTextureBuilder = nullptr;
	this->complete();
}

void MapLoader::complete()
{
#ifdef GRAPHICS
	this->propLibRegistry->destroy();
#endif

	this->propLibRegistry = nullptr;
	this->materialUsersRegistry = nullptr;
	//if (hasEventListener(Event.COMPLETE))
		//dispatchEvent(new Event(Event.COMPLETE));
}
