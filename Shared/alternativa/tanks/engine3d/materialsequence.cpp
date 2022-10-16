#include "materialsequence.h"
#include "flash/display/bitmapdata.h"
#include "flash/geom/geomrectangle.h"
#include "tankstexturematerial.h"
#include "IMaterialSequenceRegistry.h"
#include "flash/geom/matrix.h"




static const int FRAME_WIDTH_MASK = 0xFFFF;
static const int FRAME_INDEX_MASK = 0xFF0000;
static const int DISPOSE_SOURCE_MASK = (1 << 24);
static const int MIRRORED_FRAMES_MASK = (1 << 25);


MaterialSequence::MaterialSequence(IMaterialSequenceRegistry *registry,
								   BitmapData *sourceImage,
								   int frameWidth,
								   bool createMirroredFrames,
								   bool disposeSourceBitmap,
								   bool useMipMapping,
								   double mipMapResolution)
{
	this->registry = registry;
	this->sourceImage = sourceImage;
	this->frameData = frameWidth;
	if (createMirroredFrames)
	{
		this->frameData = this->frameData | MIRRORED_FRAMES_MASK;
	}
	if (disposeSourceBitmap)
	{
		this->frameData = this->frameData | DISPOSE_SOURCE_MASK;
	}
	int numFrames = sourceImage->width() / frameWidth;
	if (createMirroredFrames)
	{
		numFrames = numFrames * 2;
	}

	this->materials = new QVector<std::shared_ptr<Material>>;
	this->materials->reserve(numFrames);

	for (int i = 0;  i < numFrames; i++)
		this->materials->append(new_TanksTextureMaterial(nullptr, true, true, useMipMapping ? MipMapping::OBJECT_DISTANCE : MipMapping::NONE, mipMapResolution));

	((TanksTextureMaterial*)this->materials->at(0).get())->setTexture(this->getFrameImage(0));
}

int MaterialSequence::frameWidth()
{
	return this->frameData & FRAME_WIDTH_MASK;
}

double MaterialSequence::mipMapResolution()
{
	if (this->materials != nullptr)
	{
		return ((TextureMaterial*)this->materials->at(0).get())->resolution;
	}
	return 1;
}

void MaterialSequence::setMipMapResolution(double value)
{
	int i = 0;
	if (this->materials != nullptr)
	{
		i = 0;
		while (i < this->materials->length())
		{
			((TextureMaterial*)this->materials->at(i).get())->resolution = value;
			i++;
		}
	}
}

void MaterialSequence::release()
{
	if (this->referenceCount > 0)
	{
		this->referenceCount--;
		if (this->referenceCount == 0 && this->registry != nullptr)
		{
			this->registry->disposeSequence(this);
		}
	}
}

void MaterialSequence::dispose()
{
	for (int i = 0; i < this->materials->length(); i++)
	{
		TanksTextureMaterial *material = (TanksTextureMaterial*)this->materials->at(i).get();
		if (material != nullptr)
			material->dispose();
	}
	this->disposeSourceIfNecessary();
	this->materials = nullptr;
	this->registry = nullptr;
	this->referenceCount = 0;
}

bool MaterialSequence::tick()
{
	int frameIndex = (this->frameData & FRAME_INDEX_MASK) >> 16;
	frameIndex++;
	this->frameData = (this->frameData & (~(FRAME_INDEX_MASK))) | (frameIndex << 16);
	if (frameIndex == this->materials->length())
	{
		this->disposeSourceIfNecessary();
		return true;
	}

	TanksTextureMaterial *material = (TanksTextureMaterial*)this->materials->at(frameIndex).get();
	bool createMirroredFrames = (this->frameData & MIRRORED_FRAMES_MASK) > 0;
	if (createMirroredFrames)
	{
		if ((frameIndex & 0x01) == 1)
		{
			TanksTextureMaterial *previousMaterial = (TanksTextureMaterial*)this->materials->at(frameIndex - 1).get();
			material->setTexture(this->getMirroredImage(previousMaterial->getTexturePtr()));
		}
		else
		{
			material->setTexture(this->getFrameImage(frameIndex >> 1));
		}
	}
	else
	{
		material->setTexture(this->getFrameImage(frameIndex));
	}
	return false;
}

void MaterialSequence::disposeSourceIfNecessary()
{
	if ((this->frameData & DISPOSE_SOURCE_MASK) > 0)
	{
		//this->sourceImage->dispose(); //?
		this->frameData = this->frameData & ~DISPOSE_SOURCE_MASK;
	}
}

std::shared_ptr<BitmapData> MaterialSequence::getFrameImage(int frameIndex)
{
	int frameWidth = this->frameData & 0xFFFF;
	std::shared_ptr<BitmapData> bitmapData = std::make_shared<BitmapData>(frameWidth, this->sourceImage->height(), this->sourceImage->transparent(), 0);

	GeomRectangle rect;

	rect.x = frameIndex * frameWidth;
	rect.y = 0;
	rect.width = frameWidth;
	rect.height = this->sourceImage->height();

	Point point(0, 0);

	bitmapData->copyPixels(this->sourceImage, &rect, &point);
	return bitmapData;
}

std::shared_ptr<BitmapData> MaterialSequence::getMirroredImage(BitmapData *source)
{
	Matrix matrix(-1, 0, 0, 1);

	std::shared_ptr<BitmapData> result = std::make_shared<BitmapData>(source->width(), source->height(), source->transparent(), 0);
	matrix.tx = source->width();
	result->draw(source, &matrix);
	return result;
}
