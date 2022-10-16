#ifndef TANKSMAP_H
#define TANKSMAP_H

#include <qglobal.h>
#include "resourceloader.h"
#include <memory>

class QString;
class MapLoader;
class Config;
class CollisionPrimitive;
class KDContainer;
class Occluder;
class Object3D;
class HidableObject3DWrapper;


class TanksMap : public ResourceLoader
{
	QString *mapId;
	MapLoader *loader;

public:

	std::shared_ptr<KDContainer> mapContainer;

	TanksMap(Config *config, const QString &idMap);
	~TanksMap();
	bool run();

	QString getMapId() const;

	std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> getCollisionPrimitives();
private:
	void onLoadingComplete();
	QVector<HidableObject3DWrapper*> *hidableObjects;
};

#endif // TANKSMAP_H
