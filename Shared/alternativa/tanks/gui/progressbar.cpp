#include "progressbar.h"

void ProgressBar::repaintFill()
{
	//this->fill.graphics.clear();
	//this->fill.graphics.beginFill(this->fillColor, 1);
	//this->fill.graphics.drawRect(0, 0, Math.round((this->_progress * this->_width)), this->_height);
}

ProgressBar::ProgressBar()
{
	//mouseEnabled = false;
	//mouseChildren = false;
	//tabEnabled = false;
	//tabChildren = false;
	//this->bar = new Shape();
	//this->fill = new Shape();
	//this->fillMask = new Shape();
	//addChild(this->bar);
	//addChild(this->fill);
	//addChild(this->fillMask);
	//this->fill.mask = this->fillMask;
}

void ProgressBar::resize(int width)
{
	this->_width = width;
	//this->bar.graphics.clear();
	//this->bar.graphics.beginFill(this->barColor, 1);
	//this->bar.graphics.drawRoundRect(0, 0, this->_width, this->_height, this->_height, this->_height);
	//this->fillMask.graphics.clear();
	//this->fillMask.graphics.beginFill(0, 1);
	//this->fillMask.graphics.drawRoundRect(0, 0, this->_width, this->_height, this->_height, this->_height);
	if (this->_progress > 0)
	{
		this->setProgress(this->_progress);
	}
}

void ProgressBar::setProgress(double n)
{
	this->_progress = n;
	if (this->_width > 0)
	{
		this->repaintFill();
	}
}

double ProgressBar::width()
{
	return this->_width;
}

double ProgressBar::height()
{
	return this->_height;
}
