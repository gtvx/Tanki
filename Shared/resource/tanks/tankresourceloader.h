#ifndef TANKRESOURCELOADER_H
#define TANKRESOURCELOADER_H

#include <QString>
#include <QVector>

class TankResource;
class TankResourceList;

class TankResourceLoader
{
	QString path;
	QVector<TankResource*> queue;
	int length = 0;
	int lengthFailed = 0;
	void parse(const QByteArray &jsonData);
public:
	TankResourceList *list;
	int status = 0;
	TankResourceLoader(const QString &path);
	~TankResourceLoader();
	void clear();
	static void loadModel(TankResource *str);
	static void loadModelLocal(TankResource *tankResource);
	static void loadModelData(TankResource *tankResource, const QByteArray &data);
private:
	void loadQueue();
};

#endif // TANKRESOURCELOADER_H
