#ifndef MATERIALSEQUENCE_H
#define MATERIALSEQUENCE_H

#include <QVector>
#include <memory>

class IMaterialSequenceRegistry;
class Material;
class BitmapData;


class MaterialSequence
{
	IMaterialSequenceRegistry *registry;
	int frameData;

public:

	int referenceCount;
	QVector<std::shared_ptr<Material>> *materials;
	BitmapData *sourceImage;

	MaterialSequence(IMaterialSequenceRegistry *registry,
					 BitmapData *sourceImage,
					 int frameWidth,
					 bool createMirroredFrames,
					 bool disposeSourceBitmap,
					 bool useMipMapping,
					 double mipMapResolution);

	int frameWidth();
	double mipMapResolution();
	void setMipMapResolution(double value);
	void release();
	void dispose();
	bool tick();

private:
	void disposeSourceIfNecessary();
	std::shared_ptr<BitmapData> getFrameImage(int frameIndex);
	std::shared_ptr<BitmapData> getMirroredImage(BitmapData *source);
};

#endif // MATERIALSEQUENCE_H
