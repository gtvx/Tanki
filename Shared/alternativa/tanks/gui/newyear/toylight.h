#ifndef TOYLIGHT_H
#define TOYLIGHT_H

#include "flash/display/bitmapdata.h"

class ToyLight
{
public:

	BitmapData *bd;
	uint32_t colorLighting;

	static ToyLight *RED;
	static ToyLight *GREEN;
	static ToyLight *BLUE;
	static ToyLight *YELLOW;

	ToyLight(BitmapData *bd, uint32_t colorLighting);
	static ToyLight* getRandomToy();
};

#endif // TOYLIGHT_H
