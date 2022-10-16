#include "mainpanelrank.h"
#include "resource/resourceutil.h"
#include <QString>
#include "flash/display/stage.h"
#include "flash/display/stage3d.h"
#include "flash/display3D/context3d.h"
#include "flash/display/bitmapdata.h"
#include <QImage>

MainPanelRank::MainPanelRank(int value)
{
	this->setValue(value);
}

void MainPanelRank::setValue(int value)
{
	this->_value = value;
	if (value == 0)
		return;
	icon = ResourceUtil::getResourceImageLocal("ranks_normal/" + QString::number(value) + ".png");
}

void MainPanelRank::draw(Stage *stage)
{
	if (icon == nullptr)
		return;

	Context3D *contect = stage->stage3D()->context3D();
	QImage *image = icon->qimage();
	uint32_t *buffer = (uint32_t*)image->bits();

	contect->draw(buffer, 0, 0, 56, 64);
}
