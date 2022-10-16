#ifndef USERTITLEPROGRESSBAR_H
#define USERTITLEPROGRESSBAR_H

#include "flash/geom/geomrectangle.h"
#include <stdint.h>

class ProgressBarSkin;
class BitmapData;
class TextureMaterial;
class Graphics;


class UserTitleProgressBar
{
	int maxValue;
	int barWidth;
	int _progress;
	int x;
	int y;

	ProgressBarSkin const *skin;

public:
	UserTitleProgressBar(int x, int y, int maxValue, int barWidth, const ProgressBarSkin *skin);

	void setSkin(const ProgressBarSkin *skin);
	int progress();
	void setProgress(int value);
	void draw(uint32_t *pixels, int image_width);

};

#endif // USERTITLEPROGRESSBAR_H
