#ifndef HEALINGGUNSFXDATA_H
#define HEALINGGUNSFXDATA_H

#include <QVector>
#include <memory>

class Material;
class TextureAnimation;

class HealingGunSFXData
{
public:
	QVector<std::shared_ptr<Material>>* idleSparkMaterials;
	//idleSound:Sound;
	QVector<std::shared_ptr<Material>>* healSparkMaterials;
	QVector<std::shared_ptr<Material>>* healShaftMaterials;
	QVector<std::shared_ptr<Material>>* healShaftEndMaterials;
	//healSound:Sound;
	QVector<std::shared_ptr<Material>>* damageSparkMaterials;
	QVector<std::shared_ptr<Material>>* damageShaftMaterials;
	QVector<std::shared_ptr<Material>>* damageShaftEndMaterials;
	//damageSound:Sound;
	TextureAnimation *idleSparkData;
	TextureAnimation *healSparkData;
	TextureAnimation *healShaftEndData;
	TextureAnimation *healShaftData;
	TextureAnimation *damageSparkData;
	TextureAnimation *damageShaftData;
	TextureAnimation *damageShaftEndData;
};

#endif // HEALINGGUNSFXDATA_H
