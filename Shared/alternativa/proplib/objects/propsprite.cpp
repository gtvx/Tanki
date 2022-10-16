#include "propsprite.h"
#include "alternativa/engine3d/objects/sprite3d.h"



PropSprite::PropSprite(std::shared_ptr<TextureByteData> &textureData, double originX, double originY, double scale) :
	PropObject(PropObjectType::SPRITE)
{
	this->textureData = textureData;
	this->scale = scale;

	std::shared_ptr<Sprite3D> sprite = new_Sprite3D(1, 1, nullptr);
	sprite->originX = originX;
	sprite->originY = originY;

	std::shared_ptr<Object3D> object = sprite;

	this->setObject3D(object);
}

void PropSprite::traceProp() //override
{
	//super.traceProp();
}

void PropSprite::remove()
{
	//var s:Sprite3D = (object as Sprite3D);
	//s.destroy();
	//s = null;
}
