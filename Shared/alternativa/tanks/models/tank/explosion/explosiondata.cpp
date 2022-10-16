#include "explosiondata.h"
#include "alternativa/tanks/engine3d/materialsequence.h"

void ExplosionData::release()
{
	this->explosionSequence->release();
	this->shockWaveSequence->release();
	this->smokeSequence->release();
}
