#include "colortransformstruct.h"

ColorTransformStruct::ColorTransformStruct(double t, double redMultiplier, double greenMultiplier, double blueMultiplier, double alphaMultiplier, int redOffset, int greenOffset, int blueOffset, int alphaOffset)
{
	this->t = t;
	this->redMultiplier = redMultiplier;
	this->greenMultiplier = greenMultiplier;
	this->blueMultiplier = blueMultiplier;
	this->alphaMultiplier = alphaMultiplier;
	this->redOffset = redOffset;
	this->greenOffset = greenOffset;
	this->blueOffset = blueOffset;
	this->alphaOffset = alphaOffset;
}
