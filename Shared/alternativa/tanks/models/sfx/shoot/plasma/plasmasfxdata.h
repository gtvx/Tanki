#ifndef PLASMASFXDATA_H
#define PLASMASFXDATA_H

#include <QVector>
#include <memory>

class Material;
class ColorTransform;
class TextureAnimation;

class PlasmaSFXData
{
public:
	PlasmaSFXData();

	QVector<std::shared_ptr<Material>> *shotMaterials;
	QVector<std::shared_ptr<Material>> *explosionMaterials;
	Material *shotFlashMaterial;
	//Sound *shotSound;
	//Sound *explosionSound;
	std::shared_ptr<ColorTransform> colorTransform;
	TextureAnimation *shotData;
	TextureAnimation *explosionData;
	TextureAnimation *flashData;
};

#endif // PLASMASFXDATA_H
