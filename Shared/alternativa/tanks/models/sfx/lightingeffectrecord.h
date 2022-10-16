#ifndef LIGHTINGEFFECTRECORD_H
#define LIGHTINGEFFECTRECORD_H

#include <stdint.h>

class LightingEffectRecord
{
public:
	double attenuationBegin;
	double attenuationEnd;
	double intensity;
	uint32_t color;
	int time;

	LightingEffectRecord(double _attenuationBegin = 0,
						 double _attenuationEnd = 0,
						 uint32_t _color = 0,
						 double _intensity = 0,
						 int _time = 0)
	{
		this->attenuationBegin = _attenuationBegin;
		this->attenuationEnd = _attenuationEnd;
		this->color = _color;
		this->intensity = _intensity;
		this->time = _time;
	}
};

#endif // LIGHTINGEFFECTRECORD_H
