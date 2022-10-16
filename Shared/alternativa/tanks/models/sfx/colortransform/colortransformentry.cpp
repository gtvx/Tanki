#include "colortransformentry.h"
#include "com/alternativaplatform/projects/tanks/client/warfare/models/colortransform/struct/colortransformstruct.h"


ColorTransformEntry::ColorTransformEntry(const ColorTransformStruct &cts)
{
	this->t = cts.t;
	this->redMultiplier = cts.redMultiplier;
	this->greenMultiplier = cts.greenMultiplier;
	this->blueMultiplier = cts.blueMultiplier;
	this->alphaMultiplier = cts.alphaMultiplier;
	this->redOffset = cts.redOffset;
	this->greenOffset = cts.greenOffset;
	this->blueOffset = cts.blueOffset;
	this->alphaOffset = cts.alphaOffset;
}
