#ifndef FLAMETHROWERSFXDATA_H
#define FLAMETHROWERSFXDATA_H

#include <QVector>
#include <memory>

class ColorTransformEntry;
class TextureAnimation;
class Material;

class FlamethrowerSFXData
{
public:
	FlamethrowerSFXData();
	QVector<std::shared_ptr<Material>> *materials;
	//Sound *flameSound;
	QVector<ColorTransformEntry*> colorTransformPoints;
	std::shared_ptr<TextureAnimation> data;
};

#endif // FLAMETHROWERSFXDATA_H
