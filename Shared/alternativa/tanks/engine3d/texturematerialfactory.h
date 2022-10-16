#ifndef TEXTUREMATERIALFACTORY_H
#define TEXTUREMATERIALFACTORY_H

#include <memory>

class TextureMaterial;
class BitmapData;

class TextureMaterialFactory
{
public:
	static std::shared_ptr<TextureMaterial> createTextureMaterial(std::shared_ptr<BitmapData>, bool mipMapping);
};

#endif // TEXTUREMATERIALFACTORY_H
