#ifndef GUNSHOOTSFXDATA_H
#define GUNSHOOTSFXDATA_H

#include <QVector>
#include <memory>

class Material;
class Sound;
class TextureAnimation;


class GunShootSFXData
{
public:
	GunShootSFXData();

	Material *shotMaterial;
	//QVector<std::shared_ptr<Material>> *explosionMaterials;
	Sound *shotSound;
	Sound *explosionSound;
	TextureAnimation *shotData;
	TextureAnimation *explosionData;
};

#endif // GUNSHOOTSFXDATA_H
