#ifndef GRAPHICSUTILS_H
#define GRAPHICSUTILS_H

#include <memory>

class TextureAnimation;
class TextureMaterialRegistry;
class BitmapData;

class GraphicsUtils
{
public:	
	static void getTextureAnimation(TextureAnimation *textureAnimation, std::shared_ptr<BitmapData> &image, int frameWith, int frameHeight, int maxFrames = 0);
};

#endif // GRAPHICSUTILS_H
