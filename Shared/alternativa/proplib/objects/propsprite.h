#ifndef PROPSPRITE_H
#define PROPSPRITE_H

#include "propobject.h"

class TextureByteData;


class PropSprite : public PropObject
{
public:

	std::shared_ptr<TextureByteData> textureData;
	double scale;

	PropSprite(std::shared_ptr<TextureByteData> &textureData, double originX = 0.5, double originY = 0.5, double scale = 1);

	void traceProp();

	void remove();
};

#endif // PROPSPRITE_H
