#ifndef PROGRESSBARSKIN_H
#define PROGRESSBARSKIN_H

#include <memory>
class BitmapData;

class ProgressBarSkin
{
public:

	static const ProgressBarSkin* getHEALTHBAR_DM();
	static const ProgressBarSkin* getHEALTHBAR_BLUE();
	static const ProgressBarSkin* getHEALTHBAR_RED();
	static const ProgressBarSkin* getWEAPONBAR();


	uint32_t color;
	uint32_t bgColor;


	uint32_t const *leftTipFg;
	uint32_t const *leftTipBg;
	uint32_t const *rightTipFg;
	uint32_t const *rightTipBg;

	ProgressBarSkin(uint32_t color,
					uint32_t bgColor,
					const uint32_t *leftTipFg,
					const uint32_t *leftTipBg,
					const uint32_t *rightTipFg,
					const uint32_t *rightTipBg)
	{
		this->color = color;
		this->bgColor = bgColor;
		this->leftTipFg = leftTipFg;
		this->leftTipBg = leftTipBg;
		this->rightTipFg = rightTipFg;
		this->rightTipBg = rightTipBg;
	}

};

#endif // PROGRESSBARSKIN_H
