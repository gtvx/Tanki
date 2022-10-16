#include "graphics_debug.h"
#include "alternativa/engine3d/core/vertex.h"
#include "directxgraphics.h"
#include "alternativa/engine3d/core/wrapper.h"
#include "alternativa/physics/collision/types/boundbox.h"
#include "alternativa/math/vector3.h"
#include "mymath.h"
#include "_global.h"
#include <QDebug>

#include <d3dx9.h>
#include <d3d9.h>
#include <d3dx9core.h>
#include <d3dx9tex.h>
#include <d3d9types.h>

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

void Debug_drawBounds(DirectXGraphics *graphics, const BoundBox &b, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color)
{
	_function_name("Debug_drawBounds");

	IDirect3DDevice9Ex *device = graphics->getDevice();


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

		v.cameraX += viewSizeX;
		v.cameraY += viewSizeY;
	}

	/*
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
	*/

	(void)color;

	struct Vertex_t
	{
		float x, y, z, rhw;
		uint32_t color;
	};

	Vertex_t pVertex_1[5] = {
		{(float)v1.cameraX, (float)v1.cameraY, 0, 0, color},
		{(float)v2.cameraX, (float)v2.cameraY, 0, 0, color},
		{(float)v4.cameraX, (float)v4.cameraY, 0, 0, color},
		{(float)v3.cameraX, (float)v3.cameraY, 0, 0, color},
		{(float)v1.cameraX, (float)v1.cameraY, 0, 0, color},
	};

	Vertex_t pVertex_2[5] = {
		{(float)v5.cameraX, (float)v5.cameraY, 0, 0, color},
		{(float)v6.cameraX, (float)v6.cameraY, 0, 0, color},
		{(float)v8.cameraX, (float)v8.cameraY, 0, 0, color},
		{(float)v7.cameraX, (float)v7.cameraY, 0, 0, color},
		{(float)v5.cameraX, (float)v5.cameraY, 0, 0, color},
	};

	Vertex_t pVertex_3[2] = {
		{(float)v1.cameraX, (float)v1.cameraY, 0, 0, color},
		{(float)v5.cameraX, (float)v5.cameraY, 0, 0, color},
	};

	Vertex_t pVertex_4[2] = {
		{(float)v2.cameraX, (float)v2.cameraY, 0, 0, color},
		{(float)v6.cameraX, (float)v6.cameraY, 0, 0, color},
	};

	Vertex_t pVertex_5[2] = {
		{(float)v4.cameraX, (float)v4.cameraY, 0, 0, color},
		{(float)v8.cameraX, (float)v8.cameraY, 0, 0, color},
	};

	Vertex_t pVertex_6[2] = {
		{(float)v3.cameraX, (float)v3.cameraY, 0, 0, color},
		{(float)v7.cameraX, (float)v7.cameraY, 0, 0, color},
	};


	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, pVertex_1, sizeof(Vertex_t));
	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, pVertex_2, sizeof(Vertex_t));
	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, pVertex_3, sizeof(Vertex_t));
	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, pVertex_4, sizeof(Vertex_t));
	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, pVertex_5, sizeof(Vertex_t));
	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, pVertex_6, sizeof(Vertex_t));
}



void Debug_drawRect(DirectXGraphics *graphics, const BoundBox &b, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color)
{
	_function_name("Debug_drawRect");

	IDirect3DDevice9Ex *device = graphics->getDevice();

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

		v.cameraX += viewSizeX;
		v.cameraY += viewSizeY;
	}

	/*
	graphics->moveTo(v1.cameraX, v1.cameraY);
	graphics->lineTo(v2.cameraX, v2.cameraY);
	graphics->lineTo(v4.cameraX, v4.cameraY);
	graphics->lineTo(v3.cameraX, v3.cameraY);
	graphics->lineTo(v1.cameraX, v1.cameraY);

	graphics->end(color);
	*/

	struct Vertex_t
	{
		float x, y, z, rhw;
		uint32_t color;
	};

	Vertex_t pVertex_1[5] = {
		{(float)v1.cameraX, (float)v1.cameraY, 0, 0, color},
		{(float)v2.cameraX, (float)v2.cameraY, 0, 0, color},
		{(float)v4.cameraX, (float)v4.cameraY, 0, 0, color},
		{(float)v3.cameraX, (float)v3.cameraY, 0, 0, color},
		{(float)v1.cameraX, (float)v1.cameraY, 0, 0, color},
	};

	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, pVertex_1, sizeof(Vertex_t));
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


void Debug_draw_point(DirectXGraphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, Wrapper *wrapper, uint32_t color)
{
	_function_name("Debug_draw_point");

	IDirect3DDevice9Ex *device = graphics->getDevice();

	struct Vertex_t
	{
		float x, y, z, rhw;
		uint32_t color;
	};

	Vertex_t pVertex_1[20];


	int i = 0;

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
		if (i == 20)
			break;

		Vertex_t *v1 = &pVertex_1[i];
		v1->x = X + viewSizeX;
		v1->y = Y + viewSizeY;
		v1->z = 0;
		v1->rhw = 0;
		v1->color = color;
		i++;

		wrapper = wrapper->_next;
	}

	//graphics->end(color);

	if (i > 2)
		device->DrawPrimitiveUP(D3DPT_LINESTRIP, i-1, pVertex_1, sizeof(Vertex_t));
}


void Debug_draw_point(DirectXGraphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &v, uint32_t color)
{
	_function_name("Debug_draw_point 1");

	IDirect3DDevice9Ex *device = graphics->getDevice();

	Vector3 v0;
	v0.x = v0.y = v0.z = 0.;

	double X1, Y1;
	double X2, Y2;

	if (!Debug_get(v0, viewSizeX, viewSizeY, m, X1, Y1))
		return;

	if (!Debug_get(v, viewSizeX, viewSizeY, m, X2, Y2))
		return;

	X1 += viewSizeX;
	X2 += viewSizeX;

	Y1 += viewSizeY;
	Y2 += viewSizeY;

	struct Vertex_t
	{
		float x, y, z, rhw;
		uint32_t color;
	};

	Vertex_t pVertex_1[2] = {
		{(float)X1, (float)Y1, 0, 0, color},
		{(float)X2, (float)Y2, 0, 0, color}
	};

	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, pVertex_1, sizeof(Vertex_t));
}

void Debug_draw_point(DirectXGraphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &a, const Vector3 &b, uint32_t color)
{
	_function_name("Debug_draw_point 2");

	IDirect3DDevice9Ex *device = graphics->getDevice();

	double X1, Y1;
	double X2, Y2;

	if (!Debug_get(a, viewSizeX, viewSizeY, m, X1, Y1))
		return;

	if (!Debug_get(b, viewSizeX, viewSizeY, m, X2, Y2))
		return;

	X1 += viewSizeX;
	X2 += viewSizeX;

	Y1 += viewSizeY;
	Y2 += viewSizeY;


	struct Vertex_t
	{
		float x, y, z, rhw;
		uint32_t color;
	};

	Vertex_t pVertex_1[5] = {
		{(float)X1, (float)Y1, 0, 0, color},
		{(float)X2, (float)Y2, 0, 0, color}
	};

	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, pVertex_1, sizeof(Vertex_t));
}


void Debug_draw_string(DirectXGraphics *graphics, double viewSizeX, double viewSizeY, const Matrix4 &m, const Vector3 &pos, int offset_x, int offset_y, const QString &str, uint32_t color, const QFont &font)
{
	_function_name("Debug_draw_string");

	(void)graphics;
	(void)str;
	(void)color;
	(void)font;

	//IDirect3DDevice9Ex *device = graphics->getDevice();

	double X, Y;

	if (!Debug_get(pos, viewSizeX, viewSizeY, m, X, Y))
		return;

	X += offset_x;
	Y += offset_y;

	if (-viewSizeX >= X || viewSizeX <= X || -viewSizeY >= Y || viewSizeY <= Y) {
		return;
	}

	//graphics->drawString(str, X, Y, font);
	//graphics->endBrush(color);
}


void Debug_drawTriangle(DirectXGraphics *graphics, const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, double viewSizeX, double viewSizeY, const Matrix4 &m, uint32_t color)
{
	_function_name("Debug_drawTriangle");

	IDirect3DDevice9Ex *device = graphics->getDevice();

	double X0, Y0;
	double X1, Y1;
	double X2, Y2;

	if (!Debug_get(v0, viewSizeX, viewSizeY, m, X0, Y0))
		return;

	if (!Debug_get(v1, viewSizeX, viewSizeY, m, X1, Y1))
		return;

	if (!Debug_get(v2, viewSizeX, viewSizeY, m, X2, Y2))
		return;

	/*
	graphics->moveTo(X0, Y0);
	graphics->lineTo(X1, Y1);
	graphics->lineTo(X2, Y2);
	graphics->lineTo(X0, Y0);

	//graphics->drawString(s0, X0, Y0);
	//graphics->drawString(s1, X1 + 10, Y1);
	//graphics->drawString(s2, X2 + 20, Y2);

	graphics->end(color);
	*/

	X0 += viewSizeX;
	X1 += viewSizeX;
	X2 += viewSizeX;

	Y0 += viewSizeY;
	Y1 += viewSizeY;
	Y2 += viewSizeY;


	struct Vertex_t
	{
		float x, y, z, rhw;
		uint32_t color;
	};

	Vertex_t pVertex_1[4] = {
		{(float)X0, (float)Y0, 0, 0, color},
		{(float)X1, (float)Y1, 0, 0, color},
		{(float)X2, (float)Y2, 0, 0, color},
		{(float)X0, (float)Y0, 0, 0, color},
	};

	device->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, pVertex_1, sizeof(Vertex_t));
}
