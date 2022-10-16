#include "texturematerialfactory.h"
#include "alternativa/engine3d/materials/texturematerial.h"


std::shared_ptr<TextureMaterial> TextureMaterialFactory::createTextureMaterial(std::shared_ptr<BitmapData> bitmapData, bool mipMapping)
{
	std::shared_ptr<TextureMaterial> t = new_TextureMaterial();
	t->init(false, true, mipMapping ? MipMapping::OBJECT_DISTANCE : MipMapping::NONE);
	t->setTexture(bitmapData);
	return t;
}
