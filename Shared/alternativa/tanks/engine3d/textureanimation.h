#ifndef TEXTUREANIMATION_H
#define TEXTUREANIMATION_H

#include <QVector>
#include <memory>

class TextureMaterial;
class UVFrame;

class TextureAnimation
{
public:
	std::shared_ptr<TextureMaterial> material;
	QVector<UVFrame*> frames;
	double fps;
};

#endif // TEXTUREANIMATION_H
