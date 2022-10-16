#ifndef ISKINTEXTUREREGISTRY_H
#define ISKINTEXTUREREGISTRY_H

class TankSkinPart;
class BitmapData;

class ISkinTextureRegistry
{
public:
	virtual BitmapData* getTexture(TankSkinPart*, BitmapData*) = 0;
	virtual void releaseTexture(TankSkinPart*, BitmapData*) = 0;
	virtual void clear() = 0;
};

#endif // ISKINTEXTUREREGISTRY_H
