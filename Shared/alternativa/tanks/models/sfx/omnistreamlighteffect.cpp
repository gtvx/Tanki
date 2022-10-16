#include "omnistreamlighteffect.h"
#include "alternativa/engine3d/lights/omnilight.h"
#include "lightanimation.h"


OmniStreamLightEffect::OmniStreamLightEffect(ObjectPool *objectPool) :
	StreamLightEffect(objectPool)
{
	light = new_OmniLight(0, 0, 0);
	this->StreamLightEffect::init(light.get());
}

void OmniStreamLightEffect::init(Object3DPositionProvider *param1, LightAnimation *param2, LightAnimation *param3)
{
	this->positionProvider = param1;
	this->startTime = param2->getLiveTime();
	this->loopTime = param3->getLiveTime();
	this->startAnimation = param2;
	this->loopAnimation = param3;
	this->random = loopTime / 4;
	starting = true;
	currentTime = 0;
	alive = true;
	fading = false;
	fadeTime = 0;
}
