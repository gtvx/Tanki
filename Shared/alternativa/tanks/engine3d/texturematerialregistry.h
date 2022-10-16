#ifndef TEXTUREMATERIALREGISTRY_H
#define TEXTUREMATERIALREGISTRY_H

#include "ITextureMaterialRegistry.h"
#include "cachedentitystat.h"
#include <qhash_ptr.h>
#include <QVector>
#include <QHash>
#include <memory>

class MaterialEntry;
class MaterialEntry;



class TextureMaterialRegistry : public ITextureMaterialRegistry
{
	QVector<std::shared_ptr<TextureMaterial>> materials;
	QHash<QString, std::shared_ptr<MaterialEntry>> entryForTextureString;
	QHash<std::shared_ptr<BitmapData>, std::shared_ptr<MaterialEntry>> entryForTexture;
	QHash<TextureMaterial*, std::shared_ptr<MaterialEntry>> entryForMaterial;

	bool _useMipMapping;
	bool mipMappingEnabled;
	CachedEntityStat materialStat;

	std::shared_ptr<MaterialEntry> getOrCreateEntry(std::shared_ptr<BitmapData> &bitmapData);
	std::shared_ptr<TextureMaterial> createMaterial(std::shared_ptr<BitmapData> &bitmapData);
	std::shared_ptr<MaterialEntry> createMaterialEntry(std::shared_ptr<BitmapData> bitmapData, std::shared_ptr<TextureMaterial> &textureMaterial);
	void removeMaterialEntry(MaterialEntry *param1);
	void _clearTexture(TextureMaterial *param1);
	MaterialEntry* createPaintMaterialEntry(const QString &param1, std::shared_ptr<TextureMaterial> param2);

public:
	TextureMaterialRegistry(int value);
	void setTimerInterval(int value);
	AnimatedPaintMaterial* getAnimatedPaint(ImageResource *imageResource, std::shared_ptr<BitmapData> &lightmap, std::shared_ptr<BitmapData> &details, const QString &objId) override;
	PaintMaterial* getPaint1(ImageResource *imageResource, std::shared_ptr<BitmapData> &lightmap, std::shared_ptr<BitmapData> &details, const QString &objId) override;
	void setResoluion(double value) override;
	bool getUseMipMapping() override;
	void setUseMipMapping(bool value) override;
	TextureMaterial* getMaterial(MaterialType materialType, std::shared_ptr<BitmapData> &param1, double mipMapResolution, bool = true) override;
	QString getDump() override;
	void disposeMaterial(TextureMaterial *material) override;
	void clear() override;

protected:
	std::shared_ptr<BitmapData> getTexture(std::shared_ptr<BitmapData> &bitmapData);
	void forEachMaterial();
};

#endif // TEXTUREMATERIALREGISTRY_H
