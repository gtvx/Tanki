#include "tankresourceloader.h"
#include "tankresourcelist.h"
#include "tankresource.h"
#include "service.h"
#include "alternativa/math/vector3.h"
#include "alternativa/engine3d/loaders/parser3ds.h"
#include "alternativa/engine3d/core/camera3d.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "file.h"
#include "resourcepath.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSet>


TankResourceLoader::TankResourceLoader(const QString &path)
{
	this->path = path;

	this->list = new TankResourceList();

	QString filename = Service::getClientPath() + "/" + path;


	QByteArray data;

	if (File::read(filename, data))
	{
		parse(data);
	}
	else
	{
		qDebug("error TankResourceLoader::TankResourceLoader()");
	}
}

TankResourceLoader::~TankResourceLoader()
{
	delete this->list;
}

void TankResourceLoader::clear()
{
	this->list->clear();
}

void TankResourceLoader::parse(const QByteArray &jsonData)
{
	QJsonDocument doc = QJsonDocument::fromJson(jsonData);
	QJsonObject config = doc.object();

	if (config.value("id").toString() == "MODELS")
	{
		QJsonArray items = config.value("items").toArray();

		for (int i = 0; i < items.count(); i++)
		{
			QJsonObject obj = items.at(i).toObject();

			QString name = obj.value("name").toString();
			QString path = obj.value("src").toString();

			/*
			if (((IStorageService(Main.osgi.getService(IStorageService)).getStorage().data["use_old_textures"] == true) && ((!(path.search("hull/") == -1)) || (!(path.search("turret/") == -1)))))
			{
				oldPath = path.split("/")[1];
				newPath = (oldPath + "_old");
				path = path.replace(oldPath, newPath);
			}
			*/

			std::shared_ptr<Mesh> mesh;
			//this->queue.append(new TankResource(mesh, name, path));

			auto tankResource = new TankResource(mesh, name, path);

			list->add(tankResource, tankResource->id);

			this->length++;
		}
		//this->loadQueue();
	}
}

void TankResourceLoader::loadQueue()
{
	/*
	for (TankResource *file : this->queue)
	{
		this->loadModel(file);
	}
	*/
}



void TankResourceLoader::loadModel(TankResource *tankResource)
{
	QString url = tankResource->next;
	if (url.indexOf(QChar('?')) >= 0)
		url = url.split(QChar('?'))[0];

	QString filename = Service::getResourcesPath() + url;

	QByteArray data;
	if (!File::read(filename, data))
	{
		qDebug() << "error load" << filename;
		tankResource->error = true;
		return;
	}

	loadModelData(tankResource, data);
}


void TankResourceLoader::loadModelLocal(TankResource *tankResource)
{
	QByteArray data;

	QString filename = ResourcePath::get() + tankResource->next;

	if (!File::read(filename, data))
	{
		qDebug() << "error load" << filename;
		tankResource->error = true;
		return;
	}

	loadModelData(tankResource, data);
}

void TankResourceLoader::loadModelData(TankResource *tankResource, const QByteArray &data)
{
	Parser3DS parser;
	parser.parse(data, "");

	int len = parser.objects.length();
	if (len == 0)
	{
		tankResource->error = true;
		return;
	}


	std::shared_ptr<Vector3> flagMount = nullptr;
	std::shared_ptr<Vector3> turretMount = nullptr;

	std::shared_ptr<QVector<std::shared_ptr<Vector3>>> muzzles = std::make_shared<QVector<std::shared_ptr<Vector3>>>();

	for (int i = 0; i < parser.objects.length(); i++)
	{
		Object3D *obj = parser.objects.at(i).get();
		QString name = obj->getName();
		if (name.split("0")[0] == "muzzle")
			muzzles->append(std::make_shared<Vector3>(obj->position.x, obj->position.y, obj->position.z));

		if (name.indexOf("fmnt") >= 0)
			flagMount = std::make_shared<Vector3>(obj->position.x, obj->position.y, obj->position.z);

		if (name == "mount")
			turretMount = std::make_shared<Vector3>(obj->position.x, obj->position.y, obj->position.z);
	}


	std::shared_ptr<Object3D> object3d = parser.objects.at(0);



	std::shared_ptr<Mesh> mesh = std::dynamic_pointer_cast<Mesh>(object3d);

	tankResource->mesh = mesh;
	tankResource->muzzles = muzzles;
	tankResource->flagMount = flagMount;
	tankResource->turretMount = turretMount;

	//TankResource *tnk = new TankResource(mesh, str->id, "", muzzles, flagMount, turretMount);

	//list->add(tnk, tnk->id);

	tankResource->objects->clear();

	tankResource->objects->reserve(len);
	for (int i = 0; i < len; i++)
		tankResource->objects->append(Object3DAll_clone(parser.objects.at(i)));

	tankResource->loaded = true;
}

