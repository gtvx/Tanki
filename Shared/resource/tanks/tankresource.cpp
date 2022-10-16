#include "tankresource.h"
#include <QString>
#include <QVector>

TankResource::TankResource(std::shared_ptr<Mesh> &mesh,
						   const QString &id,
						   const QString &next,
						   std::shared_ptr<QVector<std::shared_ptr<Vector3>>> muzzles,
						   std::shared_ptr<Vector3> flagMount,
						   std::shared_ptr<Vector3> turretMount)
{
	this->mesh = mesh;
	this->id = id;
	this->next = next;
	this->muzzles = muzzles;
	this->flagMount = flagMount;
	this->turretMount = turretMount;
	this->objects = std::make_shared<QVector<std::shared_ptr<Object3D>>>();
	this->error = false;
	this->loaded = false;
}
