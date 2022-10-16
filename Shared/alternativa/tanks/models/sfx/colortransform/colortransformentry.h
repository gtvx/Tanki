#ifndef COLORTRANSFORMENTRY_H
#define COLORTRANSFORMENTRY_H

class ColorTransformStruct;

class ColorTransformEntry
{
public:

	double t;

	double redMultiplier;
	double greenMultiplier;
	double blueMultiplier;
	double alphaMultiplier;

	int redOffset;
	int greenOffset;
	int blueOffset;
	int alphaOffset;

	ColorTransformEntry(const ColorTransformStruct &cts);
};

#endif // COLORTRANSFORMENTRY_H
