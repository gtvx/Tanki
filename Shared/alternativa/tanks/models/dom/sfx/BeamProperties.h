#ifndef BEAMPROPERTIES_H
#define BEAMPROPERTIES_H

#include <memory>

class TextureMaterial;

class BeamProperties
{
public:
	std::shared_ptr<TextureMaterial> beamMaterial;
	std::shared_ptr<TextureMaterial> beamTipMaterial;
	double beamWidth;
	double unitLength;
	double animationSpeed;
	double uRange;
	double alpha;

	BeamProperties(std::shared_ptr<TextureMaterial> beamMaterial,
				   std::shared_ptr<TextureMaterial> beamTipMaterial,
				   double beamWidth,
				   double unitLength,
				   double animationSpeed,
				   double uRange,
				   double alpha)
	{
		this->beamMaterial = beamMaterial;
		this->beamTipMaterial = beamTipMaterial;
		this->beamWidth = beamWidth;
		this->unitLength = unitLength;
		this->animationSpeed = animationSpeed;
		this->uRange = uRange;
		this->alpha = alpha;
	}
};

#endif // BEAMPROPERTIES_H
