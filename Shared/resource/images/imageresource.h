#ifndef IMAGERESOURCE_H
#define IMAGERESOURCE_H

#include <QString>
#include <memory>
#include "../ImageType.h"

class QString;
class ObjectBitmapData;
class MultiframeResourceData;

class ImageResource
{	
	//Object *imageLoader;
	//Loader *nativeLoader;

public:

	std::shared_ptr<ObjectBitmapData> bitmapData;
	QString id;
	bool animatedMaterial;
	MultiframeResourceData *multiframeData;
	QString url;


	//IResourceLoadListener *completeLoadListener;

	bool isWeb;
	bool isSVG;
	ImageType type;

	ImageResource(std::shared_ptr<ObjectBitmapData> &bitmapData, const QString &id, bool animatedMaterial, MultiframeResourceData *multiframeData, const QString &url);

	void load();
	bool loaded();

private:

	void onPictureLoadingComplete(const QByteArray &data);
	void onPictureLoadingError();
};

#endif // IMAGERESOURCE_H
