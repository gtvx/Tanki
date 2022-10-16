#include "toylight.h"

//private static var SHAR_1:Class = ToyLight_SHAR_1;
//private static var SHAR_2:Class = ToyLight_SHAR_2;
//private static var SHAR_3:Class = ToyLight_SHAR_3;
//private static var SHAR_4:Class = ToyLight_SHAR_4;
//private static var toys:Array = new Array(RED, GREEN, BLUE, YELLOW);

//static ToyLight *RED = new ToyLight(new SHAR_1().bitmapData, 0xFF0000);
//static ToyLight *GREEN = new ToyLight(new SHAR_2().bitmapData, 0xFF00);
//static ToyLight *BLUE = new ToyLight(new SHAR_3().bitmapData, 82878);
//static ToyLight *YELLOW = new ToyLight(new SHAR_4().bitmapData, 0xFFA600);

ToyLight::ToyLight(BitmapData *bd, uint32_t colorLighting)
{
	this->bd = bd;
	this->colorLighting = colorLighting;
}


ToyLight* ToyLight::getRandomToy()
{
	return nullptr;
	//return (toys[Math.floor((Math.random() * toys.length))]);
}
