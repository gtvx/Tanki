#ifndef RICOCHETSFXDATA_H
#define RICOCHETSFXDATA_H

#include <QVector>
#include <memory>

class TextureAnimation;
class Material;
class ColorTransform;

class RicochetSFXData
{
public:
	QVector<std::shared_ptr<Material>> *shotMaterials;
	QVector<std::shared_ptr<Material>> *ricochetFlashMaterials;
	QVector<std::shared_ptr<Material>> *explosionMaterials;
	Material *tailTrailMaterial;
	Material *shotFlashMaterial;
	//Sound shotSound:Sound;
	//Sound ricochetSound:Sound;
	//Sound explosionSound;
	std::shared_ptr<ColorTransform> colorTransform;
	TextureAnimation *dataFlash;
	TextureAnimation *dataExplosion;
	TextureAnimation *dataShot;
};

#endif // RICOCHETSFXDATA_H
