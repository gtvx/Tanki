#include "tankresourcelist.h"
#include "tankresource.h"
#include <QDebug>
#include <QHash>

TankResourceList::TankResourceList()
{
	this->models = new QHash<QString, TankResource*>;
}

TankResourceList::~TankResourceList()
{	
	this->clear();
	delete this->models;
}

void TankResourceList::add(TankResource *model, const QString &id)
{
	if (model == nullptr) {
		throw 4341;
	}

	if (this->models->contains(id) == false)
	{
		//if (model->mesh == nullptr)
			//throw 452434;

		this->models->insert(id, model);
	}
	else
	{
		//trace("Model arleady registered!");
	}
}

TankResource* TankResourceList::getModel(const QString &key)
{
	if (this->models->contains(key) == false) {
		qDebug() << "error" << key;
		return nullptr;
	}

	return this->models->value(key);
}

void TankResourceList::clear()
{
	QHashIterator<QString, TankResource*> i(*this->models);
	while (i.hasNext())
	{
		i.next();
		TankResource *resource = i.value();
		resource->loaded = false;
		resource->error = false;
		resource->mesh = nullptr;
		resource->flagMount = nullptr;
		resource->turretMount = nullptr;
		resource->muzzles = nullptr;
	}
}

