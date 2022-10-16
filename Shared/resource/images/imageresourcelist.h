#ifndef IMAGERESOURCELIST_H
#define IMAGERESOURCELIST_H

#include <QVector>
#include <QHash>

class ImageResource;

class ImageResourceList
{
	//QVector<QString> ids;
public:

	QHash<QString, ImageResource*> images;
	ImageResourceList();

	void add(ImageResource *img);
	ImageResource* getImage(const QString &key);
	bool isLoaded(const QString &key);
	void clear();
};

#endif // IMAGERESOURCELIST_H
