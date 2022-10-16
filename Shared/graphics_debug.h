#ifndef DEBUG_H
#define DEBUG_H

#include "alternativa/engine3d/core/object3d.h"
#include "stdint.h"

class Vertex;
class Wrapper;
class QFont;
class DirectXGraphics;

void Debug_drawBounds(DirectXGraphics*, const BoundBox&, double, double, const Matrix4&, uint32_t color);
void Debug_drawRect(DirectXGraphics*, const BoundBox &b, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color);
void Debug_drawTriangle(DirectXGraphics*, const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color);
void Debug_draw_point(DirectXGraphics*, double viewSizeX, double viewSizeY, const Matrix4&, Wrapper *wrapper, uint32_t color);
void Debug_draw_point(DirectXGraphics*, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &v, uint32_t color);
void Debug_draw_point(DirectXGraphics*, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &a, const Vector3 &b, uint32_t color);
void Debug_draw_string(DirectXGraphics*, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &pos, int offset_x, int offset_y, const QString &str, uint32_t color, const QFont &font);

#endif // DEBUG_H
