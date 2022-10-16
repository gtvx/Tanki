#ifndef PROPLIBRARY_H
#define PROPLIBRARY_H

#include <memory>

class PropGroup;
class ByteArrayMap;
class TextureByteDataMap;
class QString;
class QDomElement;
class PropMesh;
class PropSprite;
class PropObject;
class PropState;
class PropData;


class PropLibrary
{
	std::shared_ptr<PropGroup> _rootGroup;
	std::shared_ptr<ByteArrayMap> files;
	std::shared_ptr<TextureByteDataMap> imageMap;

public:
	PropLibrary(std::shared_ptr<ByteArrayMap> &files);
	static void prepareMeshes(PropGroup *param1);
	QString getName();
	PropGroup* getRootGroup();
	void freeMemory();
private:
	std::shared_ptr<TextureByteDataMap> parseImageMap(const QDomElement &imagesXml);
	std::shared_ptr<PropMesh> parsePropMesh(const QDomElement &propXml);
	std::shared_ptr<PropSprite> parsePropSprite(const QDomElement &propXml);
	std::shared_ptr<PropObject> parsePropObject(const QDomElement &parentXmlElement);
	std::shared_ptr<PropState> parseState(const QDomElement &stateXml);
	std::shared_ptr<PropData> parseProp(const QDomElement &propXml);
	std::shared_ptr<PropGroup> parseGroup(const QDomElement &groupXML);
};

#endif // PROPLIBRARY_H
