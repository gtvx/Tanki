#ifndef TANKRESOURCELIST_H
#define TANKRESOURCELIST_H

#include <qglobal.h>

class TankResource;

class TankResourceList
{
	QHash<QString, TankResource*> *models;
public:
	TankResourceList();
	~TankResourceList();

	void add(TankResource *model, const QString &id);
	TankResource *getModel(const QString &key);
	void clear();
};

#endif // TANKRESOURCELIST_H
