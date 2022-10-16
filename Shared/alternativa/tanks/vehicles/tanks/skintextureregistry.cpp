#include "skintextureregistry.h"

/*
class TextureEntry
{
public:
	int refCount;
	BitmapData *texture;

	TextureEntry(BitmapData *texture)
	{
		this->texture = texture;
		this->refCount = 0;
	}
};
*/


SkinTextureRegistry::SkinTextureRegistry()
{

}

BitmapData *SkinTextureRegistry::getTexture(TankSkinPart *tankPart, BitmapData *colormap)
{
	(void)tankPart;
	(void)colormap;

	return nullptr;

/*
	Dictionary partTextures = this->textures[tankPart->partId];
	if (partTextures == nullptr)
	{
		partTextures = new Dictionary();
		this.textures[tankPart.partId] = partTextures;
	}

	TextureEntry *textureEntry = partTextures[colormap];
	if (textureEntry == nullptr)
	{
		textureEntry = new TextureEntry(tankPart.createTexture(colormap, false));
		partTextures[colormap] = textureEntry;
	}

	textureEntry->refCount++;
	return textureEntry.texture;
	*/
}

void SkinTextureRegistry::releaseTexture(TankSkinPart *tankPart, BitmapData *colormap)
{
	(void)tankPart;
	(void)colormap;

	/*
	Dictionary *partTextures = this->textures[tankPart.partId];

	if (partTextures == nullptr)
	{
		return;
	}

	TextureEntry *textureEntry = partTextures[colormap];
	if (textureEntry == nullptr)
	{
		return;
	}

	textureEntry->refCount--;

	if (textureEntry->refCount == 0)
	{
		textureEntry->texture->dispose();
		delete partTextures[colormap];
	}
	*/
}

void SkinTextureRegistry::clear()
{

}
