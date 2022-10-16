#include "usertitleprogressbar.h"
#include "progressbarskin.h"

namespace
{
	const uint32_t image_barShadowLeft[32] =
	{
		0x05000000, 0x0F000000, 0x25000000, 0x3F000000,
		0x0F000000, 0x3B000000, 0x85000000, 0xB9000000,
		0x21000000, 0x80000000, 0xDB000000, 0xF9000000,
		0x33000000, 0xA9000000, 0xF5000000, 0xFF000000,
		0x33000000, 0xA9000000, 0xF5000000, 0xFF000000,
		0x21000000, 0x80000000, 0xDB000000, 0xF9000000,
		0x0F000000, 0x3B000000, 0x85000000, 0xB9000000,
		0x05000000, 0x0F000000, 0x25000000, 0x3F000000,
	};

	const uint32_t image_barShadowRight[32] =
	{
		0x3F000000, 0x25000000, 0x0F000000, 0x05000000,
		0xB9000000, 0x85000000, 0x3B000000, 0x0F000000,
		0xF9000000, 0xDB000000, 0x80000000, 0x21000000,
		0xFF000000, 0xF5000000, 0xA9000000, 0x33000000,
		0xFF000000, 0xF5000000, 0xA9000000, 0x33000000,
		0xF9000000, 0xDB000000, 0x80000000, 0x21000000,
		0xB9000000, 0x85000000, 0x3B000000, 0x0F000000,
		0x3F000000, 0x25000000, 0x0F000000, 0x05000000,
	};


	const int barTipWidth = 2;
	const int barHeight = 4;
	const int shadowTipWidth = 4;
	const int shadowHeight = 8;
	const int barOffsetX = shadowTipWidth - barTipWidth; //2
	const int barOffsetY = (shadowHeight - barHeight) >> 1; //2



	void draw_line(uint32_t *pixels, int image_width, int begin, int length, uint32_t color)
	{
		uint32_t *p = pixels + (image_width * barOffsetY) + (barOffsetX + begin);;
		color = 0xFF000000 | color;
		uint32_t *p0 = p;
		uint32_t *p1 = p + image_width;
		uint32_t *p2 = p + (image_width * 2);
		uint32_t *p3 = p + (image_width * 3);
		for (int i = 0; i < length; i++)
			p0[i] = p1[i] = p2[i] = p3[i] = color;
	}

	void draw_w2_h4(uint32_t *pixels, int image_width, int x, int y, const uint32_t *in)
	{
		uint32_t *p = pixels + (image_width * y) + x;
		for (int i = 0, j = 0; i < 4; i++)
		{
			p[0] = in[j++];
			p[1] = in[j++];
			p += image_width;
		}
	}

	void draw_y0_w4_h8(uint32_t *pixels, int image_width, int x, const uint32_t *in)
	{
		uint32_t *p = pixels + x;
		for (int i = 0, j = 0; i < 8; i++)
		{
			p[0] = in[j++];
			p[1] = in[j++];
			p[2] = in[j++];
			p[3] = in[j++];
			p += image_width;
		}
	}
}

UserTitleProgressBar::UserTitleProgressBar(int x, int y, int maxValue, int barWidth, const ProgressBarSkin *skin)
{
	this->x = x;
	this->y = y;
	this->maxValue = maxValue;
	this->barWidth = barWidth;
	this->setSkin(skin);
}

void UserTitleProgressBar::setSkin(const ProgressBarSkin *skin)
{
	this->skin = skin;
}

int UserTitleProgressBar::progress()
{
	return this->_progress;
}

void UserTitleProgressBar::setProgress(int value)
{
	if (value < 0)
	{
		value = 0;
	}
	else
	{
		if (value > this->maxValue)
		{
			value = this->maxValue;
		}
	}
	this->_progress = value;
}

void UserTitleProgressBar::draw(uint32_t *pixels, int image_width)
{
	pixels += (this->y * image_width) + this->x;

	draw_y0_w4_h8(pixels, image_width, 0, image_barShadowLeft);

	{
		uint32_t *p = pixels + shadowTipWidth;

		uint32_t *p0 = p;
		uint32_t *p1 = p + image_width;
		//uint32_t *p2 = p + (image_width * 2);
		//uint32_t *p3 = p + (image_width * 3);
		//uint32_t *p4 = p + (image_width * 4);
		//uint32_t *p5 = p + (image_width * 5);
		uint32_t *p6 = p + (image_width * 6);
		uint32_t *p7 = p + (image_width * 7);

		int len = this->barWidth - (2 * barTipWidth);
		for (int i = 0; i < len; i++)
		{
			p0[i] = 0x4E000000;
			p1[i] = 0xCF000000;
			//p2[i] = 0xFE000000;
			//p3[i] = 0xFF000000;
			//p4[i] = 0xFF000000;
			//p5[i] = 0xFE000000;
			p6[i] = 0xCF000000;
			p7[i] = 0x4E000000;
		}
	}


	draw_y0_w4_h8(pixels,
				  image_width,
				  (shadowTipWidth + this->barWidth) - (2 * barTipWidth),
				  image_barShadowRight);

	int displayWidth = int((this->barWidth * this->_progress) / this->maxValue);
	int w = this->barWidth - barTipWidth;
	int bgStart = 0;

	if (displayWidth >= barTipWidth)
	{
		if (displayWidth == this->barWidth)
		{
			draw_w2_h4(pixels, image_width, barOffsetX, barOffsetY, this->skin->leftTipFg);
			draw_line(pixels, image_width, barTipWidth, w - barTipWidth, this->skin->color);
			draw_w2_h4(pixels, image_width, barOffsetX + w, barOffsetY, this->skin->rightTipFg);
			bgStart = displayWidth;
		}
		else
		{
			draw_w2_h4(pixels, image_width, barOffsetX, barOffsetY, skin->leftTipFg);

			if (displayWidth > barTipWidth)
			{
				if (displayWidth > w)
					displayWidth = w;

				bgStart = displayWidth;
				draw_line(pixels, image_width, barTipWidth, displayWidth - barTipWidth, skin->color);
			}
			else
			{
				bgStart = barTipWidth;
			}
		}
	}

	if (bgStart == 0)
	{
		draw_w2_h4(pixels, image_width, barOffsetX, barOffsetY, this->skin->leftTipBg);
		draw_line(pixels, image_width, barTipWidth, w - barTipWidth, this->skin->bgColor);
		draw_w2_h4(pixels, image_width, barOffsetX + w, barOffsetY, this->skin->rightTipBg);
	}
	else
	{
		if (bgStart < this->barWidth)
		{
			draw_line(pixels, image_width, bgStart, w - bgStart, this->skin->bgColor);
			draw_w2_h4(pixels, image_width, barOffsetX + w, barOffsetY, skin->rightTipBg);
		}
	}
}
