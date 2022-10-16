#ifndef COLORTRANSFORMSTRUCT_H
#define COLORTRANSFORMSTRUCT_H


class ColorTransformStruct
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

	ColorTransformStruct(double t,
						 double redMultiplier,
						 double greenMultiplier,
						 double blueMultiplier,
						 double alphaMultiplier,
						 int redOffset,
						 int greenOffset,
						 int blueOffset,
						 int alphaOffset);
};

#endif // COLORTRANSFORMSTRUCT_H
