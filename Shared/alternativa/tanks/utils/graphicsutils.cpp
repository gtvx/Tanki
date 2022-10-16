#include "graphicsutils.h"
#include <QVector>
#include "alternativa/tanks/engine3d/uvframe.h"
#include "mymath.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/engine3d/textureanimation.h"
#include "flash/display/bitmapdata.h"

static QVector<UVFrame*> getUVFramesFromTexture(BitmapData *texture, int frameWidth, int frameHeight, int maxFrames = 0)
{
	double textureWidth = texture->width();
	double actualFrameWidth = MyMath::min(frameWidth, textureWidth);
	int numColumns = int((textureWidth / actualFrameWidth));
	double textureHeight = texture->height();
	double actualFrameHeight = MyMath::min(frameHeight, textureHeight);
	int numRows = int((textureHeight / actualFrameHeight));
	int numFrames = (numColumns * numRows);


	if (maxFrames > 0 && numFrames > maxFrames)
	{
		numFrames = maxFrames;
	}

	QVector<UVFrame*> frames;
	int frameIndex = 0;
	int rowIndex = 0;

	while (rowIndex < numRows)
	{
		int topY = rowIndex * actualFrameHeight;
		int bottomY = topY + actualFrameHeight;
		int columIndex = 0;
		while (columIndex < numColumns)
		{
			double leftX = columIndex * actualFrameWidth;
			double rightX = leftX + actualFrameWidth;

			frames.append(new UVFrame((leftX / textureWidth), (topY / textureHeight), (rightX / textureWidth), (bottomY / textureHeight)));
			if (frameIndex == numFrames)
			{
				return frames;
			}
			columIndex++;
		}
		rowIndex++;
	}
	return frames;
}



void GraphicsUtils::getTextureAnimation(TextureAnimation *textureAnimation,
										std::shared_ptr<BitmapData> &image,
										int frameWith,
										int frameHeight,
										int maxFrames)
{
	textureAnimation->material = new_TextureMaterial();
	textureAnimation->material->init();
	textureAnimation->material->setTexture(image);
	textureAnimation->frames = getUVFramesFromTexture(image.get(), frameWith, frameHeight, maxFrames);
	textureAnimation->fps = 0;
}
