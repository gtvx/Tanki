#ifndef UVFRAME_H
#define UVFRAME_H

class UVFrame
{
public:
	double topLeftU;
	double topLeftV;
	double bottomRightU;
	double bottomRightV;

	UVFrame(double topLeftU, double topLeftV, double bottomRightU, double bottomRightV);
};

#endif // UVFRAME_H
