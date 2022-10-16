#ifndef OMNISTREAMLIGHTEFFECT_H
#define OMNISTREAMLIGHTEFFECT_H

#include "streamlighteffect.h"
#include <memory>
class OmniLight;

class OmniStreamLightEffect : public StreamLightEffect
{
	std::shared_ptr<OmniLight> light;
public:
	OmniStreamLightEffect(ObjectPool *objectPool);
	void init(Object3DPositionProvider *param1, LightAnimation *param2, LightAnimation *param3);
};

#endif // OMNISTREAMLIGHTEFFECT_H
