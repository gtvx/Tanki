#include "debug.h"
#include "alternativa/engine3d/core/vertex.h"
#include "graphics.h"
#include "alternativa/engine3d/core/wrapper.h"
#include "alternativa/physics/collision/types/boundbox.h"
#include "alternativa/math/vector3.h"
#include "mymath.h"
#include "_global.h"
#include "alternativa/math/matrix4.h"
#include <QDebug>

namespace
{
	struct _Vertex
	{
		double x, y, z;
		double cameraX;
		double cameraY;
		double cameraZ;
	};
}

void Debug_drawBounds(Graphics *graphics, const BoundBox &b, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color)
{
	_function_name("Debug_drawBounds");

	_Vertex boundVertexList[8];


	_Vertex &v1 = boundVertexList[0];
	v1.x = b.MinX;
	v1.y = b.MinY;
	v1.z = b.MinZ;

	_Vertex &v2 = boundVertexList[1];
	v2.x = b.MaxX;
	v2.y = b.MinY;
	v2.z = b.MinZ;

	_Vertex &v3 = boundVertexList[2];
	v3.x = b.MinX;
	v3.y = b.MaxY;
	v3.z = b.MinZ;

	_Vertex &v4 = boundVertexList[3];
	v4.x = b.MaxX;
	v4.y = b.MaxY;
	v4.z = b.MinZ;

	_Vertex &v5 = boundVertexList[4];
	v5.x = b.MinX;
	v5.y = b.MinY;
	v5.z = b.MaxZ;

	_Vertex &v6 = boundVertexList[5];
	v6.x = b.MaxX;
	v6.y = b.MinY;
	v6.z = b.MaxZ;

	_Vertex &v7 = boundVertexList[6];
	v7.x = b.MinX;
	v7.y = b.MaxY;
	v7.z = b.MaxZ;

	_Vertex &v8 = boundVertexList[7];
	v8.x = b.MaxX;
	v8.y = b.MaxY;
	v8.z = b.MaxZ;





	for (int i = 0; i < 8; i++)
	{
		_Vertex &v = boundVertexList[i];

		v.cameraX = (m.ma * v.x) + (m.mb * v.y) + (m.mc * v.z) + m.md;
		v.cameraY = (m.me * v.x) + (m.mf * v.y) + (m.mg * v.z) + m.mh;
		v.cameraZ = (m.mi * v.x) + (m.mj * v.y) + (m.mk * v.z) + m.ml;
		if (v.cameraZ <= 0.)
			return;
	}


	for (int i = 0; i < 8; i++)
	{
		_Vertex &v = boundVertexList[i];

		double s = 1. / v.cameraZ;
		v.cameraX *= viewSizeX * s;
		v.cameraY *= viewSizeY * s;
	}

	graphics->moveTo(v1.cameraX, v1.cameraY);
	graphics->lineTo(v2.cameraX, v2.cameraY);
	graphics->lineTo(v4.cameraX, v4.cameraY);
	graphics->lineTo(v3.cameraX, v3.cameraY);
	graphics->lineTo(v1.cameraX, v1.cameraY);


	graphics->moveTo(v5.cameraX, v5.cameraY);
	graphics->lineTo(v6.cameraX, v6.cameraY);
	graphics->lineTo(v8.cameraX, v8.cameraY);
	graphics->lineTo(v7.cameraX, v7.cameraY);
	graphics->lineTo(v5.cameraX, v5.cameraY);

	graphics->moveTo(v1.cameraX, v1.cameraY);
	graphics->lineTo(v5.cameraX, v5.cameraY);

	graphics->moveTo(v2.cameraX, v2.cameraY);
	graphics->lineTo(v6.cameraX, v6.cameraY);

	graphics->moveTo(v4.cameraX, v4.cameraY);
	graphics->lineTo(v8.cameraX, v8.cameraY);

	graphics->moveTo(v3.cameraX, v3.cameraY);
	graphics->lineTo(v7.cameraX, v7.cameraY);

	graphics->end(color);
}



void Debug_drawRect(Graphics *graphics, const BoundBox &b, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color)
{
	_function_name("Debug_drawRect");

	_Vertex boundVertexList[4];

	_Vertex &v1 = boundVertexList[0];
	v1.x = b.MinX;
	v1.y = b.MinY;
	v1.z = b.MinZ;

	_Vertex &v2 = boundVertexList[1];
	v2.x = b.MaxX;
	v2.y = b.MinY;
	v2.z = b.MinZ;

	_Vertex &v3 = boundVertexList[2];
	v3.x = b.MinX;
	v3.y = b.MaxY;
	v3.z = b.MinZ;

	_Vertex &v4 = boundVertexList[3];
	v4.x = b.MaxX;
	v4.y = b.MaxY;
	v4.z = b.MinZ;


	for (int i = 0; i < 4; i++)
	{
		_Vertex &v = boundVertexList[i];
		v.cameraX = (m.ma * v.x) + (m.mb * v.y) + (m.mc * v.z) + m.md;
		v.cameraY = (m.me * v.x) + (m.mf * v.y) + (m.mg * v.z) + m.mh;
		v.cameraZ = (m.mi * v.x) + (m.mj * v.y) + (m.mk * v.z) + m.ml;
		if (v.cameraZ <= 0.)
			return;
	}

	double centerX =  m.ma + m.mb + m.mc + m.md;
	double centerY =  m.me + m.mf + m.mg + m.mh;
	double centerZ =  m.mi + m.mj + m.mk + m.ml;

	{
		double b = 1. / centerZ;
		centerX *= viewSizeX * b;
		centerY *= viewSizeY * b;
	}


	for (int i = 0; i < 4; i++)
	{
		_Vertex &v = boundVertexList[i];
		double b = 1. / v.cameraZ;
		v.cameraX *= viewSizeX * b;
		v.cameraY *= viewSizeY * b;
	}

	graphics->moveTo(v1.cameraX, v1.cameraY);
	graphics->lineTo(v2.cameraX, v2.cameraY);
	graphics->lineTo(v4.cameraX, v4.cameraY);
	graphics->lineTo(v3.cameraX, v3.cameraY);
	graphics->lineTo(v1.cameraX, v1.cameraY);

	graphics->end(color);
}




static bool Debug_get(const Vector3 &v, double viewSizeX, double viewSizeY, const Matrix4 &m, double &_x, double &_y)
{
	_function_name("Debug_get");

	double X = (m.ma * v.x) + (m.mb * v.y) + (m.mc * v.z) + m.md;
	double Y = (m.me * v.x) + (m.mf * v.y) + (m.mg * v.z) + m.mh;
	double Z = (m.mi * v.x) + (m.mj * v.y) + (m.mk * v.z) + m.ml;
	if (Z <= 0)
		return false;

	double c = 1. / Z;
	_x = X * viewSizeX * c;
	_y = Y * viewSizeY * c;
	return true;
}


void Debug_draw_point(Graphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, Wrapper *wrapper, uint32_t color)
{
	_function_name("Debug_draw_point");

	bool first = true;
	while (wrapper != nullptr) {
		Vertex *vertex = wrapper->vertex.get();

		Vector3 v;
		v.x = vertex->x;
		v.y = vertex->y;
		v.z = vertex->z;
		double X, Y;
		if (!Debug_get(v, viewSizeX, viewSizeY, m, X, Y)) {
			wrapper = wrapper->_next;
			continue;
		}

		if (first == true) {
			graphics->moveTo(X, Y);
			first = false;
		} else {
			graphics->lineTo(X, Y);
		}

		wrapper = wrapper->_next;
	}

	graphics->end(color);
}


void Debug_draw_point(Graphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &v, uint32_t color)
{
	_function_name("Debug_draw_point 1");

	Vector3 v0;
	v0.x = v0.y = v0.z = 0.;

	double X1, Y1;
	double X2, Y2;

	if (!Debug_get(v0, viewSizeX, viewSizeY, m, X1, Y1))
		return;

	if (!Debug_get(v, viewSizeX, viewSizeY, m, X2, Y2))
		return;

	graphics->moveTo(X1, Y1);
	graphics->lineTo(X2, Y2);

	graphics->end(color);
}

void Debug_draw_point(Graphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &a, const Vector3 &b, uint32_t color)
{
	_function_name("Debug_draw_point 2");

	double X1, Y1;
	double X2, Y2;

	if (!Debug_get(a, viewSizeX, viewSizeY, m, X1, Y1))
		return;

	if (!Debug_get(b, viewSizeX, viewSizeY, m, X2, Y2))
		return;

	graphics->moveTo(X1, Y1);
	graphics->lineTo(X2, Y2);

	graphics->end(color);
}


void Debug_draw_string(Graphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &pos, int offset_x, int offset_y, const QString &str, uint32_t color, const QFont &font)
{
	_function_name("Debug_draw_string");

	double X, Y;

	if (!Debug_get(pos, viewSizeX, viewSizeY, m, X, Y))
		return;

	X += offset_x;
	Y += offset_y;

	if (-viewSizeX >= X || viewSizeX <= X || -viewSizeY >= Y || viewSizeY <= Y) {
		return;
	}

	graphics->drawString(str, X, Y, font);
	graphics->endBrush(color);
}


void Debug_drawTriangle(Graphics *graphics, const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color)
{
	_function_name("Debug_drawTriangle");

	double X0, Y0;
	double X1, Y1;
	double X2, Y2;

	if (!Debug_get(v0, viewSizeX, viewSizeY, m, X0, Y0))
		return;

	if (!Debug_get(v1, viewSizeX, viewSizeY, m, X1, Y1))
		return;

	if (!Debug_get(v2, viewSizeX, viewSizeY, m, X2, Y2))
		return;

	graphics->moveTo(X0, Y0);
	graphics->lineTo(X1, Y1);
	graphics->lineTo(X2, Y2);
	graphics->lineTo(X0, Y0);

	//graphics->drawString(s0, X0, Y0);
	//graphics->drawString(s1, X1 + 10, Y1);
	//graphics->drawString(s2, X2 + 20, Y2);

	graphics->end(color);
}
