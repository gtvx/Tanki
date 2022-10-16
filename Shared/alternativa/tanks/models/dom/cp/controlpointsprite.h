#ifndef CONTROLPOINTSPRITE_H
#define CONTROLPOINTSPRITE_H

#include "alternativa/math/vector3.h"
#include "alternativa/engine3d/objects/sprite3d.h"
//#include <QHash>


class TextureMaterial;
class BitmapData;
class BitmapTextureResource;

class ControlPointSprite : public Sprite3D
{
	//QHash<int, std::shared_ptr<BitmapTextureResource>> resources;
	std::shared_ptr<BitmapTextureResource> resources[210];
public:

	static const T *TYPE;
	const T *__type_next;



	Vector3 pos;
	double progress;
	double last_progress;
	int currChar;
	std::shared_ptr<TextureMaterial> tm;

	//public var mask:DrawMask = new DrawMask(w);

	ControlPointSprite(const TT &t, int pointId);

	static void init();
	static void _destroy();
	static std::shared_ptr<BitmapData> getTexture(BitmapData *in, BitmapData *abds);
	void drawChar(BitmapData *param1);
	void redraw();
};

std::shared_ptr<ControlPointSprite> new_ControlPointSprite(int pointId);

#endif // CONTROLPOINTSPRITE_H
