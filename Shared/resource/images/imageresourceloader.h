#ifndef IMAGERESOURCELOADER_H
#define IMAGERESOURCELOADER_H

#include <QVector>
#include <QHash>

class Dictionary;
class ImageResourceList;
class ImageResource;

class ImageResourceLoader
{
	int length;
	int lengthFailed;

	QHash<QString, ImageResource*> config;

public:

	QString path;
	ImageResourceList *list;
	ImageResourceList *inbattleList;


	QVector<ImageResource*> queue;
	int status;

	ImageResourceLoader(const QString &path);
	~ImageResourceLoader();

	void clear();

private:

	void load();

	void parse(const QByteArray &data);
	void loadQueue();
	void loadImage(ImageResource *img);

public:
	void loadForBattle(const QString &id);

private:
	void loadImageSingle(ImageResource *img);
};

#endif // IMAGERESOURCELOADER_H
