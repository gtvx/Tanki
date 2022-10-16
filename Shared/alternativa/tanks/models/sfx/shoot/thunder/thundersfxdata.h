#ifndef THUNDERSFXDATA_H
#define THUNDERSFXDATA_H

#include <QVector>
#include <memory>

class Material;


class ThunderSFXData
{
public:
	Material *shotMaterial;
	QVector<std::shared_ptr<Material>> *explosionMaterials;
	//Sound *shotSound;
	//Sound *explosionSound;

	ThunderSFXData();
};

#endif // THUNDERSFXDATA_H
