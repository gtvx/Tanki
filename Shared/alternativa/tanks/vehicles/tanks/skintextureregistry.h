#ifndef SKINTEXTUREREGISTRY_H
#define SKINTEXTUREREGISTRY_H

#include "ISkinTextureRegistry.h"
#include <QHash>

class SkinTextureRegistry : public ISkinTextureRegistry
{
	//QHash<, > textures;
public:
	SkinTextureRegistry();

	BitmapData* getTexture(TankSkinPart *tankPart, BitmapData *colormap) override;

	void releaseTexture(TankSkinPart *tankPart, BitmapData *colormap) override;

	void clear() override;
};

#endif // SKINTEXTUREREGISTRY_H
