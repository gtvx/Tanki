#ifndef TANKRESOURCE_H
#define TANKRESOURCE_H

#include <QString>
#include <QVector>
#include <memory>

class Mesh;
class Vector3;
class Object3D;


class TankResource
{
public:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Vector3> flagMount;
	std::shared_ptr<Vector3> turretMount;
	std::shared_ptr<QVector<std::shared_ptr<Vector3>>> muzzles;
	std::shared_ptr<QVector<std::shared_ptr<Object3D>>> objects;
	QString id;
	QString next;
	bool error;
	bool loaded;

	TankResource(std::shared_ptr<Mesh> &mesh,
				 const QString &id,
				 const QString &next,
				 std::shared_ptr<QVector<std::shared_ptr<Vector3>>> muzzles = nullptr,
				 std::shared_ptr<Vector3> flagMount = nullptr,
				 std::shared_ptr<Vector3> turretMount = nullptr);
};

#endif // TANKRESOURCE_H
