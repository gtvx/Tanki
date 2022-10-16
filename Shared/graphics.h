#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef unsigned int uint;
class QPainter;
class QPainterPath;
class QString;
class QFont;

class Graphics
{
	QPainter *painter;
	QPainterPath *painter_path;
public:
	Graphics();
	void setPainter(QPainter*);
	void beginFill(uint color, double alpha = 1.0);
	void moveTo(double x, double y);
	void lineTo(double x, double y);
	void drawString(const QString& text, int x, int y, const QFont &font);
	void end(uint color);
	void endBrush(uint color);
};

#endif // GRAPHICS_H
