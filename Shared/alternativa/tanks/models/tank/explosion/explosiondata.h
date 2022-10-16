#ifndef EXPLOSIONDATA_H
#define EXPLOSIONDATA_H

#include <memory>

class MaterialSequence;
class TextureAnimation;


class ExplosionData
{
public:
	MaterialSequence *explosionSequence;
	MaterialSequence *shockWaveSequence;
	MaterialSequence *smokeSequence;
	std::shared_ptr<TextureAnimation> explosionData;
	std::shared_ptr<TextureAnimation> shockData;
	std::shared_ptr<TextureAnimation> smokeData;

	ExplosionData(MaterialSequence *explosionSequence,
				  MaterialSequence *shockWaveSequence,
				  MaterialSequence *smokeSequence)
	{
		this->explosionSequence = explosionSequence;
		this->shockWaveSequence = shockWaveSequence;
		this->smokeSequence = smokeSequence;
	}

	void release();
};

#endif // EXPLOSIONDATA_H
