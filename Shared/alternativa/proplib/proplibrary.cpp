#include "proplibrary.h"
#include "types/propgroup.h"
#include "alternativa/utils/texturebytedata.h"
#include "objects/propsprite.h"
#include "alternativa/proplib/utils/texturebytedatamap.h"
#include "types/propstate.h"
#include "objects/propmesh.h"
#include "alternativa/utils/bytearraymap.h"
#include "types/propdata.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "mymath.h"
#include "hash.h"
#include <QDomElement>
#include <QDomNodeList>
#include <QDebug>


static const QString LIB_FILE_NAME = "library.xml";
static const QString IMG_FILE_NAME = "images.xml";


std::shared_ptr<TextureByteDataMap> PropLibrary::parseImageMap(const QDomElement &imagesXml)
{
	std::shared_ptr<TextureByteDataMap> imageFiles = std::make_shared<TextureByteDataMap>();


	QDomNodeList images = imagesXml.elementsByTagName("image");
	for (int i = 0; i < images.count(); i++)
	{
		const QDomElement image = images.at(i).toElement();


		QString originalTextureFileName = image.attribute("name");

		QString diffuseName = image.attribute("new-name").toLower();
		QString opacityName = image.attribute("alpha");

		if (!opacityName.isNull())
		{
			opacityName = opacityName.toLower();
		}


		std::shared_ptr<TextureByteData> data = std::make_shared<TextureByteData>(this->files->getValue(diffuseName), this->files->getValue(opacityName));
		imageFiles->putValue(originalTextureFileName, data);
	}

	return imageFiles;
}



std::shared_ptr<PropMesh> PropLibrary::parsePropMesh(const QDomElement &propXml)
{
	QByteArray modelData = this->files->getValue(propXml.attribute("file").toLower());

	QDomNodeList textures = propXml.elementsByTagName("texture");

	QHash<QString, QString> textureFiles;

	if (textures.count() > 0)
	{
		for (int i = 0; i < textures.count(); i++)
		{
			QDomElement texture = textures.at(i).toElement();
			textureFiles[texture.attribute("name")] = texture.attribute("diffuse-map");
		}
	}

	QString objectName = propXml.attribute("object");
	return std::make_shared<PropMesh>(modelData, objectName, textureFiles, this->files.get(), this->imageMap.get());

	/*
	var textureXml:XML;
	var modelData:ByteArray = this->files.getValue(propXml.@file.toString().toLowerCase());
	var textureFiles:Object;
	if (propXml.texture.length() > 0)
	{
		textureFiles = {}
		for each (textureXml in propXml.texture)
		{
			textureFiles[XMLUtils.copyXMLString(textureXml.@name)] = textureXml.attribute("diffuse-map").toString().toLowerCase();
		}
	}
	var objectName:String = XMLUtils.getAttributeAsString(propXml, "object", null);
	return (new PropMesh(modelData, objectName, textureFiles, this->files, this->imageMap));
	*/
}

std::shared_ptr<PropSprite> PropLibrary::parsePropSprite(const QDomElement &propXml)
{
	QString textureFile = propXml.attribute("file").toLower();

	std::shared_ptr<TextureByteData> textureData;

	if (this->imageMap == nullptr)
	{
		textureData = std::make_shared<TextureByteData>(this->files->getValue(textureFile), QByteArray());
	}
	else
	{
		textureData = this->imageMap->getValue(textureFile);
	}

	double originX = propXml.attribute("origin-x", "0.5").toDouble();
	double originY = propXml.attribute("origin-y", "0.5").toDouble();
	double scale = propXml.attribute("scale", "1").toDouble();

	return std::make_shared<PropSprite>(textureData, originX, originY, scale);
}


std::shared_ptr<PropObject> PropLibrary::parsePropObject(const QDomElement &parentXmlElement)
{

	QDomElement mesh = parentXmlElement.firstChildElement("mesh");
	if (!mesh.isNull())
	{
		return parsePropMesh(mesh);
	}

	QDomElement sprite = parentXmlElement.firstChildElement("sprite");
	if (!sprite.isNull())
	{
		return parsePropSprite(sprite);
	}

	throw 45365;
	/*
	if (parentXmlElement.mesh.length() > 0)
	{
		return (this->parsePropMesh(parentXmlElement.mesh[0]));
	}
	if (parentXmlElement.sprite.length() > 0)
	{
		return (this->parsePropSprite(parentXmlElement.sprite[0]));
	}
	throw (new Error("Unknown prop type"));
	*/
	return nullptr;
}


std::shared_ptr<PropState> PropLibrary::parseState(const QDomElement &stateXml)
{

	std::shared_ptr<PropState> state = std::make_shared<PropState>();

	QDomNodeList lods = stateXml.elementsByTagName("lod");
	if (lods.count() > 0)
	{
		for (int i = 0; i < lods.count(); i++)
		{
			QDomElement lodXml = lods.at(i).toElement();

			QDomElement element_distance = lodXml.firstChildElement("distance");
			double distance = element_distance.text().toDouble();

			std::shared_ptr<PropObject> p = parsePropObject(lodXml);

			state->addLOD(p, distance);
		}
	}
	else
	{
		std::shared_ptr<PropObject> p = parsePropObject(stateXml);

		state->addLOD(p, 0);
	}

	/*
	var lodXml:XML;

	var lods:XMLList = stateXml.lod;
	if (lods.length() > 0)
	{
		for each (lodXml in lods)
		{
			state.addLOD(this->parsePropObject(lodXml), Number(lodXml.@distance));
		}
	}
	else
	{
		state.addLOD(this->parsePropObject(stateXml), 0);
	}
	*/

	return state;
}

std::shared_ptr<PropData> PropLibrary::parseProp(const QDomElement &propXml)
{

	QString name = propXml.attribute("name");

	std::shared_ptr<PropData> prop = std::make_shared<PropData>(name);

	QDomNodeList states = propXml.elementsByTagName("state");

	if (states.count() > 0)
	{
		for (int i = 0; i < states.count(); i++)
		{
			QDomElement stateXml = states.at(i).toElement();
			prop->addState(stateXml.attribute("name"), parseState(stateXml));
		}
	}
	else
	{
		prop->addState(PropState::DEFAULT_NAME, parseState(propXml));
	}

	/*
	var stateXml:XML;
	var prop:PropData = new PropData(XMLUtils.copyXMLString(propXml.@name));
	var states:XMLList = propXml.state;
	if (states.length() > 0)
	{
		for each (stateXml in states)
		{
			prop.addState(XMLUtils.copyXMLString(stateXml.@name), this->parseState(stateXml));
		}
	}
	else
	{
		prop.addState(PropState.DEFAULT_NAME, this->parseState(propXml));
	}
	*/

	return prop;
}



std::shared_ptr<PropGroup> PropLibrary::parseGroup(const QDomElement &groupXML)
{
	QString name = groupXML.attribute("name");
	if (name.isEmpty())
		return nullptr;


	QDomElement library = groupXML.firstChildElement("library");

	std::shared_ptr<PropGroup> group = std::make_shared<PropGroup>(name);




	QDomNodeList props = groupXML.elementsByTagName("prop");
	for (int i = 0; i < props.count(); i++)
	{
		auto p = parseProp(props.at(i).toElement());
		group->addProp(p);
	}

	QDomNodeList prop_group = groupXML.elementsByTagName("prop-group");

	for (int i = 0; i < prop_group.count(); i++)
	{
		QDomElement groupElement = prop_group.at(i).toElement();

		auto p = parseGroup(groupElement);
		group->addGroup(p);
	}

	/*
	for each (propElement in groupXML.prop)
	{
		group.addProp(this->parseProp(propElement));
	}

	for each (groupElement in groupXML.elements("prop-group"))
	{
		group.addGroup(this->parseGroup(groupElement));
	}
	*/

	return group;
}





PropLibrary::PropLibrary(std::shared_ptr<ByteArrayMap> &files)
{
	imageMap = nullptr;

	if (files == nullptr)
	{
		//throw (new ArgumentError("Parameter files is null"));
		throw 14134132;
	}

	this->files = files;

	QByteArray imageMapData = files->getValue(IMG_FILE_NAME);

	if (!imageMapData.isEmpty())
	{
		QDomDocument doc;
		if (doc.setContent(imageMapData) == false) {
			qDebug("error PropLibrary::PropLibrary() 1");
			return;
		}

		QDomElement xml = doc.documentElement();

		this->imageMap = this->parseImageMap(xml);
	}


	QByteArray d = files->getValue(LIB_FILE_NAME);


	QDomDocument doc;
	if (doc.setContent(d) == false) {
		qDebug("error PropLibrary::PropLibrary() 2");
		return;
	}

	QDomElement xml = doc.documentElement();
	this->_rootGroup = this->parseGroup(xml);
	this->prepareMeshes(this->_rootGroup.get());
}

void PropLibrary::prepareMeshes(PropGroup *param1)
{
	if (param1->groups != nullptr)
	{
		for (std::shared_ptr<PropGroup> &propGroup : *param1->groups)
		{
			prepareMeshes(propGroup.get());
		}
	}

	if (param1->props != nullptr)
	{
		for (std::shared_ptr<PropData> &propData : *param1->props)
		{
			std::shared_ptr<PropObject> propObject = propData->getDefaultState()->getDefaultObject();

			if (propObject == nullptr)
			{
				qDebug("error PropLibrary::prepareMeshes 1");
			}
			else
			{
				std::shared_ptr<Object3D> o = propObject->getObject3D();

				if (o->isType(Mesh::TYPE))
				{
					std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(o);
					if (mesh != nullptr)
					{
						mesh->calculateFacesNormals();
						mesh->calculateVerticesNormalsByAngle((65. / 180.) * MyMath::PI, 0.01);
					}
				}
			}
		}
	}

	/*
	var _loc2_:PropGroup;
	var _loc3_:PropData;
	var _loc4_:PropObject;

	if (param1.props != nullptr)
	{
		for each (_loc3_ in param1.props)
		{
			_loc4_ = _loc3_.getDefaultState().getDefaultObject();
			if ((_loc4_.object is Mesh))
			{
				Mesh(_loc4_.object).calculateFacesNormals();
				Mesh(_loc4_.object).calculateVerticesNormalsByAngle(((65 / 180) * Math.PI), 0.01);
			}
		}
	}
	*/
}

QString PropLibrary::getName()
{
	if (this->_rootGroup == nullptr)
		return QString();

	return this->_rootGroup->name;
}

PropGroup *PropLibrary::getRootGroup()
{
	return this->_rootGroup.get();
}

void PropLibrary::freeMemory()
{
	this->files->destroy();
}
