#include "graphics.h"
#include <QPainter>
#include <QPainterPath>


Graphics::Graphics()
{
	this->painter_path = new QPainterPath;
}

void Graphics::setPainter(QPainter *painter)
{
	this->painter = painter;
}

void Graphics::beginFill(uint color, double)
{
	painter->setPen(QPen(color));
}

void Graphics::moveTo(double x, double y)
{
	painter_path->moveTo(x, y);
}

void Graphics::lineTo(double x, double y)
{
	painter_path->lineTo(x, y);
}

void Graphics::drawString(const QString &text, int x, int y, const QFont &font)
{
	painter_path->addText(x, y, font, text);
}

void Graphics::end(uint color)
{
	painter->setPen(QPen(QColor(color)));
	painter->drawPath(*painter_path);
	this->painter_path->~QPainterPath();
	new(this->painter_path) QPainterPath;
}

void Graphics::endBrush(uint color)
{
	QBrush brush;
	brush.setColor(color);
	brush.setStyle(Qt::SolidPattern);
	painter->fillPath(*painter_path, brush);
	this->painter_path->~QPainterPath();
	new(this->painter_path) QPainterPath;
}
