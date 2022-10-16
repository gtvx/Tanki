#include "tanksmap.h"
#include "alternativa/engine3d/core/object3d.h"
#include "alternativa/tanks/config/loaders/maploader.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "config.h"
#include "service.h"
#include "alternativa/engine3d/containers/kdcontainer.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidableobject3dwrapper.h"
#include "alternativa/tanks/models/battlefield/hidableobjects/hidablegraphicobjects.h"
#include "_global.h"
#include <QString>
#include <QDebug>

//KDContainer ConflictContainer Object3DContainer Object3D


TanksMap::TanksMap(Config *config, const QString &idMap) :
	ResourceLoader("Tank map loader", config)
{
	this->hidableObjects = new QVector<HidableObject3DWrapper*>;
	this->mapId = new QString(idMap);
	this->loader = nullptr;
	this->mapContainer = nullptr;
}

TanksMap::~TanksMap()
{
#ifdef GRAPHICS
	if (mapContainer != nullptr)
	{
		mapContainer->destroyTree();
		mapContainer->destroy();
	}
#endif
	delete this->mapId;
	if (loader != nullptr)
		delete loader;

	qDeleteAll(*hidableObjects);
	delete this->hidableObjects;
}

bool TanksMap::run()
{
	this->loader = new MapLoader();
	//this->loader.addEventListener(Event.COMPLETE, this.onLoadingComplete);
	if (!this->loader->load(Service::getResourcesPath() + "maps/" + *this->mapId + ".xml", config->propLibRegistry)) { //config.propLibRegistry
		qDebug() << "error load map " + *this->mapId;
		return false;
	}

	this->onLoadingComplete();

	//QVector<std::shared_ptr<Object3D>> objects = this->loader->objects();
	//QVector<std::shared_ptr<Occluder>> occluders = this->loader->occluders();

	//this->mapContainer = this->createKDContainer(&objects, &occluders);
	//this.mapContainer.threshold = 0.1;
	//this.mapContainer.ignoreChildrenInCollider = true;
	//this.mapContainer.calculateBounds();
	//BattlefieldModel *bfModel = Service::getBattlefieldModel();


	//this.mapContainer.name = "Visual Kd-tree";

	//for (std::shared_ptr<Sprite3D> &sprite : *this->loader->sprites)
	//{
		//this.mapContainer.addChild(sprite);
		//bfModel->getHidableGraphicObjects()->add(new HidableObject3DWrapper(sprite.get()));
	//}
	//bfModel.build(this.mapContainer, this.collisionPrimitives, this.loader.lights);
	//bfModel->build(nullptr, this->getCollisionPrimitives(), nullptr, nullptr);
	//completeTask();
	//(Main.osgi.getService(IConsole) as IConsole).addLine("loaded map");
	return true;
}

QString TanksMap::getMapId() const
{
	return  *mapId;
}

std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> TanksMap::getCollisionPrimitives()
{
	return this->loader->getCollisionPrimitives();
}

#ifdef GRAPHICS
static std::shared_ptr<KDContainer> createKDContainer(QVector<std::shared_ptr<Object3D>> *objects, QVector<std::shared_ptr<Occluder>> *occluders)
{
	std::shared_ptr<KDContainer> container = new_KDContainer();
	container->createTree(objects, occluders);
	return container;
}
#endif




void TanksMap::onLoadingComplete()
{
	BattlefieldModel *bfModel = Service::getBattlefieldModel();

#ifdef GRAPHICS
	this->mapContainer = createKDContainer(this->loader->objects(), this->loader->occluders());
	this->mapContainer->threshold = 0.1;
	this->mapContainer->ignoreChildrenInCollider = true;
	this->mapContainer->calculateBounds();

	this->mapContainer->setName("Visual Kd-tree");


	for (std::shared_ptr<Sprite3D> sprite : *this->loader->sprites())
	{
		this->mapContainer->addChild(sprite.get());
		auto hidableObject3DWrapper = new HidableObject3DWrapper(sprite.get());
		hidableObjects->append(hidableObject3DWrapper);
		bfModel->getHidableGraphicObjects()->add(hidableObject3DWrapper);
	}
#endif


	bfModel->build(this->mapContainer, this->getCollisionPrimitives(), this->loader->lights(), nullptr);
	//completeTask();

	delete loader;
	loader = nullptr;

}
