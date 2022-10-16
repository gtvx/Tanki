#ifndef FREEZESFXDATA_H
#define FREEZESFXDATA_H

#include <QVector>
#include <memory>

class Material;
class ColorTransformEntry;


class FreezeSFXData
{
public:
	QVector<std::shared_ptr<Material>> *particleMaterials;
	QVector<std::shared_ptr<Material>> *planeMaterials;
	//Sound shotSound;
	QVector<ColorTransformEntry*> colorTransformPoints;
	double particleSpeed;
};

#endif // FREEZESFXDATA_H
