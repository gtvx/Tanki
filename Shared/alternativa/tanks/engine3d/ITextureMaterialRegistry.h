#ifndef ITEXTUREMATERIALREGISTRY_H
#define ITEXTUREMATERIALREGISTRY_H

#include <memory>

class AnimatedPaintMaterial;
class ImageResource;
class BitmapData;
class QString;
class TextureMaterial;
enum class MaterialType;
class PaintMaterial;


class ITextureMaterialRegistry
{
public:
	virtual AnimatedPaintMaterial* getAnimatedPaint(ImageResource*, std::shared_ptr<BitmapData>&, std::shared_ptr<BitmapData>&, const QString &) = 0;
	virtual PaintMaterial* getPaint1(ImageResource*, std::shared_ptr<BitmapData>&, std::shared_ptr<BitmapData>&, const QString &) = 0;
	virtual void setResoluion(double) = 0;
	virtual bool getUseMipMapping() = 0;
	virtual void setUseMipMapping(bool) = 0;
	virtual TextureMaterial* getMaterial(MaterialType, std::shared_ptr<BitmapData> &, double, bool = true) = 0;
	virtual void clear() = 0;
	virtual QString getDump() = 0;
	virtual void disposeMaterial(TextureMaterial*) = 0;
};


#endif // ITEXTUREMATERIALREGISTRY_H
