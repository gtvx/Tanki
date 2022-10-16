#ifndef SHAFTSFXDATA_H
#define SHAFTSFXDATA_H

#include <memory>

class Material;
class TextureAnimation;

class ShaftSFXData
{
public:
	//Sound zoomModeSound;
	//Sound targetingSound;
	//Sound shotSound;
	//Sound explosionSound;
	std::shared_ptr<Material> trailMaterial;
	double trailLength = 500;
	TextureAnimation *explosionAnimation;
	TextureAnimation *muzzleFlashAnimation;
};

#endif // SHAFTSFXDATA_H
