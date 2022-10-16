#ifndef RESOURCEUTIL_H
#define RESOURCEUTIL_H

#include "ResourceType.h"
#include <memory>

class QString;
class TankResourceLoader;
class TankParts;
class TankResource;
class ImageResource;
class BitmapData;


class ResourceUtil
{
public:

	static bool isInBattle;

	ResourceUtil();
	static TankResource* getResourceModel(const QString &key);
	static TankResource *getResourceModelLocal(const QString &key);
	static ImageResource* getResourceImage(const QString &key);
	static bool isDataPresent(TankParts *parts);
	static void loadResource();
	static std::shared_ptr<BitmapData> getResourceImageLocal(const QString &filename);
	static std::shared_ptr<BitmapData> getResourceImageLocal(const char *filename);
	static void clear();
};

#endif // RESOURCEUTIL_H
