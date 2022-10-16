#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <stdint.h>

class ProgressBar
{
	const uint32_t barColor = 0;
	const uint32_t fillColor = 0x33CC00;

	//private var bar:Shape;
	//private var fill:Shape;
	//private var fillMask:Shape;
	int _width;
	int _height = 5;
	double _progress = 0;

	void repaintFill();
public:
	ProgressBar();

	void resize(int width);
	void setProgress(double n);
	double width();
	double height();
};

#endif // PROGRESSBAR_H
